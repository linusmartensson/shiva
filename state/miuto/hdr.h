#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"music.h"
#include"shaders.h"
#include"framebuffer.h"
#include"image.h"
#include"camera.h"
#include"state/common/bases/fssplayer.h"
#include"state/common/bases/drawpath.h"
using namespace shiva;

class root : public shiva::behaviour {
	struct rootdata{
		FSShader fss;
		FrameBuffer fb;
		Texture color;
		Texture depth;
		bool re;
		camera cam;
		float time, ptime;
		std::vector<float> wform, spectrum;
	
		std::map<std::string, Texture*> *tx;

		rootdata() : fss("statedata/miuto/hdr.frag"), color(1, GL_RGBA16F, GL_TEXTURE_2D), depth(1, GL_DEPTH_COMPONENT32F, GL_TEXTURE_2D), re(false), time(0.f), ptime(-1.f) {
			tx = new std::map<std::string, Texture*>;
		}
		void rebuild(){
			re = false;
			time = ptime = -1.0;
			fss.rebuild();
		}
	};

	std::vector<float> points(std::function<float (glm::vec3)> density, glm::vec3 magnitude, glm::vec3 off, int count) const {
		
		std::vector<float> pts;

		int fail = 0;
		while(count > 0){
			float x = rand()/(float)RAND_MAX*magnitude.x;
			float y = rand()/(float)RAND_MAX*magnitude.y;
			float z = rand()/(float)RAND_MAX*magnitude.z;
			auto s = glm::vec3(x,y,z);
			if(density(s) > 0.0f || fail > 10) {
				pts.push_back(x+off.x);
				pts.push_back(y+off.y);
				pts.push_back(z+off.z);
				count--;
				fail = 0;
			} 
			fail++;
		}
		return pts;
	}

	//From a set of points, connect all points within a threshold in an index list.
	std::vector<uint> lines(std::vector<float> points, std::function<float (glm::vec3, glm::vec3)> theta) const {
		
		std::vector<uint> lines;
		int count = 0;


		for(int i=0;i<points.size()/3; ++i){
			for(int j=i+1;j<points.size()/3; ++j){
				if(theta(glm::vec3(points[i*3], points[i*3+1], points[i*3+2]),glm::vec3(points[j*3],points[j*3+1],points[j*3+2]))>0.f){
					lines.push_back(i);
					lines.push_back(j);
				}
			}
		}
		Log::errlog()<<"lines generated"<<std::endl;
		return lines;
	}

	void init(shiva::state *s, boost::any &data) const {
		data = rootdata();
		auto d = boost::any_cast<rootdata&>(data);

		s->reg("resourcechange");
		s->reg("music/time");
		s->reg("music/wform");
		s->reg("music/spectrum");
		s->reg("kinput");
		s->reg("init");

		d.fb.colorbuffer(0, d.color);
		d.fb.depthbuffer(d.depth);
		s->create("music", "musicplayer", std::string("statedata/miuto/demokompo.mp3"));
		std::map<std::string, float> overlayp;

		(*d.tx)["text1"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1280, 360);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 360, 0, GL_RGBA, GL_UNSIGNED_BYTE, PNGImage("statedata/miuto/text1.png").data());
		(*d.tx)["text2"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1280, 360);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 360, 0, GL_RGBA, GL_UNSIGNED_BYTE, PNGImage("statedata/miuto/text2.png").data());
		(*d.tx)["text3"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1280, 360);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 360, 0, GL_RGBA, GL_UNSIGNED_BYTE, PNGImage("statedata/miuto/text3.png").data());
		(*d.tx)["text4"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1280, 360);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 360, 0, GL_RGBA, GL_UNSIGNED_BYTE, PNGImage("statedata/miuto/text4.png").data());
		(*d.tx)["text5"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1280, 360);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 360, 0, GL_RGBA, GL_UNSIGNED_BYTE, PNGImage("statedata/miuto/text5.png").data());
		(*d.tx)["text6"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1280, 360);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 360, 0, GL_RGBA, GL_UNSIGNED_BYTE, PNGImage("statedata/miuto/text6.png").data());
		(*d.tx)["text7"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1280, 360);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 360, 0, GL_RGBA, GL_UNSIGNED_BYTE, PNGImage("statedata/miuto/text7.png").data());
		(*d.tx)["text8"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1280, 360);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 360, 0, GL_RGBA, GL_UNSIGNED_BYTE, PNGImage("statedata/miuto/text8.png").data());
		(*d.tx)["text9"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1280, 360);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 360, 0, GL_RGBA, GL_UNSIGNED_BYTE, PNGImage("statedata/miuto/text9.png").data());

		(*d.tx)["wform"] = new Texture(1, GL_R16F, GL_TEXTURE_2D, 1024, 1);
		(*d.tx)["spectrum"] = new Texture(1, GL_R16F, GL_TEXTURE_2D, 1024, 1);

		overlayp["on"] = 0.0;
		overlayp["off"] = 74.0;
		s->create("first", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/background2.frag"), std::string("music/time"), overlayp, *d.tx));
		
		overlayp["on"] = 33.0;
		overlayp["off"] = 45.0;
		s->create("first2", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/background.frag"), std::string("music/time"), overlayp, *d.tx));

		overlayp["on"] = 11.0;
		overlayp["off"] = 76.0;
		
		std::map<std::string, Buffer<float>*> path;

		path["position"] = new Buffer<float>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 3);
		std::vector<float> f;

		auto c = points([&](glm::vec3 v){
			return sin(cos(v.z/30.0)+cos(v.x/57.0+v.y/84.0)*sin(v.z/40.0)*2.0+sin(v.y*0.03)*0.5+cos(v.x*0.05)*1.2);
		}, glm::vec3(12000.f,2000.f,4000.f), glm::vec3(-1000.f,-1000.f,0.f), 12000);

		path["position"]->data(c);

		auto a = drawpath::pathdata("statedata/miuto/path1.frag","statedata/miuto/path1.geom","statedata/miuto/path1.vert",
			std::string("music/time"), std::string("camera"), overlayp, path, std::map<std::string, Texture*>(), GL_LINES);
		auto dd = drawpath::pathdata("statedata/miuto/path2.frag","statedata/miuto/path2.geom","statedata/miuto/path2.vert",
			std::string("music/time"), std::string("camera"), overlayp, path, std::map<std::string, Texture*>(), GL_LINES);
		auto q = lines(c, [&](glm::vec3 v, glm::vec3 w){
			return -glm::distance(v, w) + abs(sin(v.x*0.005))*100.0+abs(cos(w.x*0.0025))*100.0 + abs(cos(w.z*0.005))*100.0+abs(cos(v.z*0.0025))*100.0;
		});
		auto b = Buffer<uint>(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, 1);
		
		
		b.data(q);
		a.va.indices(b);
		dd.va.indices(b);
		s->create("p0", "drawpath", a);
		s->create("p1", "drawpath", dd);

		
		overlayp["on"] = 2.0;
		overlayp["off"] = 12.0;
		s->create("text1", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/overlay1.frag"), std::string("music/time"), overlayp, *d.tx));
		overlayp["on"] = 15.0;
		overlayp["off"] = 25.0;
		s->create("text2", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/overlay4.frag"), std::string("music/time"), overlayp, *d.tx));
		overlayp["on"] = 25.0;
		overlayp["off"] = 33.0;
		s->create("text3", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/overlay3.frag"), std::string("music/time"), overlayp, *d.tx));
		overlayp["on"] = 30.0;
		overlayp["off"] = 37.0;
		s->create("text4", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/overlay2.frag"), std::string("music/time"), overlayp, *d.tx));
		overlayp["on"] = 38.0;
		overlayp["off"] = 45.0;
		s->create("text5", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/overlay5.frag"), std::string("music/time"), overlayp, *d.tx));
		
		overlayp["on"] = 45.0;
		overlayp["off"] = 51.0;
		s->create("text6", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/overlay6.frag"), std::string("music/time"), overlayp, *d.tx));
		
		overlayp["on"] = 51.0;
		overlayp["off"] = 70.0;
		s->create("text7", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/overlay7.frag"), std::string("music/time"), overlayp, *d.tx));
		s->create("text8", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/overlay8.frag"), std::string("music/time"), overlayp, *d.tx));
		s->create("text9", "fssplayer", fssplayer::fssdata(std::string("statedata/miuto/overlay9.frag"), std::string("music/time"), overlayp, *d.tx));
		
		path["position"] = new Buffer<float>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 3);
		f.clear();
		for(int i=0;i<10000;++i){
			f.push_back(-0.6);
			f.push_back(-0.6);
			f.push_back(i*5.0);

			f.push_back(0.6);
			f.push_back(-0.6);
			f.push_back(i*5.0);
		}
		path["position"]->data(f);

		overlayp["on"] = 5.0;
		overlayp["off"] = 11.0;
		overlayp["offset"] = 0.0;
		s->create("cubes1", "drawpath", drawpath::pathdata("statedata/miuto/cube1.frag","statedata/miuto/cube1.geom","statedata/miuto/cube1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 1.0;
		s->create("cubes2", "drawpath", drawpath::pathdata("statedata/miuto/cube1.frag","statedata/miuto/cube1.geom","statedata/miuto/cube1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 2.0;
		s->create("cubes3", "drawpath", drawpath::pathdata("statedata/miuto/cube1.frag","statedata/miuto/cube1.geom","statedata/miuto/cube1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 3.0;
		s->create("cubes4", "drawpath", drawpath::pathdata("statedata/miuto/cube1.frag","statedata/miuto/cube1.geom","statedata/miuto/cube1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["on"] = 33.0;
		overlayp["off"] = 72.0;
		overlayp["offset"] = 0.0;
		s->create("cubes5", "drawpath", drawpath::pathdata("statedata/miuto/cube2.frag","statedata/miuto/cube2.geom","statedata/miuto/cube2.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 1.0;
		s->create("cubes6", "drawpath", drawpath::pathdata("statedata/miuto/cube2.frag","statedata/miuto/cube2.geom","statedata/miuto/cube2.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 2.0;
		s->create("cubes7", "drawpath", drawpath::pathdata("statedata/miuto/cube2.frag","statedata/miuto/cube2.geom","statedata/miuto/cube2.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 3.0;
		s->create("cubes8", "drawpath", drawpath::pathdata("statedata/miuto/cube2.frag","statedata/miuto/cube2.geom","statedata/miuto/cube2.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		

	}

	virtual void uninit(shiva::state *instance, boost::any &data) const {}

	void render(shiva::childvec children, boost::any &data) const {
				auto &d = boost::any_cast<rootdata&>(data);
		if(d.re) d.rebuild();
		
		if(d.wform.size() >= 1024 && d.tx->count("wform")){
			(*d.tx)["wform"]->bind();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 1024, 1, 0, GL_RED, GL_FLOAT, &d.wform[0]);
		}
		if(d.spectrum.size() >= 1024 && d.tx->count("spectrum")){
			(*d.tx)["spectrum"]->bind();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 1024, 1, 0, GL_RED, GL_FLOAT, &d.spectrum[0]);
		}
		d.fb.render([&]{
			glClearColor(0.0,0.0,0.0,0.0);
			glClearDepth(0.0);
			glDepthFunc(GL_GREATER);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(true);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();}); 
			glDisable(GL_BLEND);
		});
		d.fb.render([&]{
			glClearColor(0.0,0.0,0.0,0.0);
			glClearDepth(0.0);
			glDepthFunc(GL_GREATER);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(false);
			glClear(GL_COLOR_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();}); 
			glDisable(GL_BLEND);
		});
		d.fss.set("time", d.time);
		d.fss.set("res", glm::vec4((float)shiva::core::width(), (float)shiva::core::height(), 1.f/shiva::core::width(), 1.f/shiva::core::height()));
		d.fss.program().use();
		d.fss.program().bindTexture("color", d.color, 0);
		d.fss.program().bindTexture("depth", d.depth, 1);
		d.fss.program().bindTexture("wform", *(*d.tx)["wform"], 2);
		d.fss.display();
	}
	
	void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		auto &d = boost::any_cast<rootdata&>(data);
		
			
		if(received.count("kinput")){
			if(shiva::core::down(GLFW_KEY_RIGHT)){
				shiva::core::on("music/goto", (float)d.time+1);
			}
			if(shiva::core::down(GLFW_KEY_LEFT)){
				shiva::core::on("music/goto", (float)d.time-1);
			}
		}

		if(received.count("music/wform")){
			auto i = boost::any_cast<std::vector<float>&>(received["music/wform"]);

			d.wform.clear();
			for(auto j = i.begin(); j!=i.end(); ++j){
				d.wform.push_back(*j);
			}
		}
		
		if(received.count("music/spectrum")){
			auto i = boost::any_cast<std::vector<float>&>(received["music/spectrum"]);

			d.spectrum.clear();
			for(auto j = i.begin(); j!=i.end(); ++j){
				d.spectrum.push_back(*j);
			}
		}
		if(received.count("resourcechange")){
			d.re = true;
			shiva::core::on("music/goto", (float)0);
			d.cam.rebuild();
		}
		if(received.count("init")){
			
			shiva::core::on("music/play", (int)0);
			shiva::core::on("music/goto", (float)0);
			d.cam.init(instance, "statedata/miuto/camera.0");
		}
		
		if(received.count("kinput")){
			std::pair<int,bool> g = boost::any_cast<std::pair<int,bool>>(received["kinput"]);
			if(g.first == GLFW_KEY_ESC)
				shiva::core::stop();
			if(g.first == GLFW_KEY_SPACE && g.second == true){
				shiva::core::on("music/play", (int)0);
			}
		}
		if(received.count("music/time")){
			float ttime = d.time;
			d.time = boost::any_cast<float>(received["music/time"]);
			
			if(!d.re && d.time < d.ptime) shiva::core::stop();
			d.ptime = ttime;
			float speed = 0.01;

			shiva::core::on("camera", glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.cam.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.cam.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.cam.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));
			shiva::core::on("camera2", glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));

			shiva::core::on("text1/trans", 
				glm::scale(glm::mat4(1.f), glm::vec3(1280.0/3,360.0/3,100.0))
				*glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.cam.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.cam.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.cam.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::translate(glm::mat4(1.0f), glm::vec3(0.f,100.f,-500.f))
				*glm::mat4_cast(glm::rotate(glm::quat(), -0.f, glm::vec3(0,1,0)))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));
			shiva::core::on("text2/trans", 
				glm::scale(glm::mat4(1.f), glm::vec3(1280.0/3,360.0/3,100.0))
				*glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.cam.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.cam.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.cam.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::translate(glm::mat4(1.0f), glm::vec3(1000.f,100.f,-500.f))
				*glm::mat4_cast(glm::rotate(glm::quat(), -25.f, glm::vec3(-0.8,0.5,0)))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));
			shiva::core::on("text3/trans", 
				glm::scale(glm::mat4(1.f), glm::vec3(1280.0/3,360.0/3,100.0))
				*glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.cam.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.cam.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.cam.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::translate(glm::mat4(1.0f), glm::vec3(2000.f,100.f,-500.f))
				*glm::mat4_cast(glm::rotate(glm::quat(), 30.f, glm::vec3(1,1,1)))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));
			shiva::core::on("text4/trans", 
				glm::scale(glm::mat4(1.f), glm::vec3(1280.0/3,360.0/3,100.0))
				*glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.cam.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.cam.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.cam.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::translate(glm::mat4(1.0f), glm::vec3(3000.f,100.f,-500.f))
				*glm::mat4_cast(glm::rotate(glm::quat(), -13.f, glm::vec3(0,1,1)))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));
			shiva::core::on("text5/trans", 
				glm::scale(glm::mat4(1.f), glm::vec3(1280.0/3,360.0/3,100.0))
				*glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.cam.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.cam.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.cam.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::translate(glm::mat4(1.0f), glm::vec3(4000.f,100.f,-500.f))
				*glm::mat4_cast(glm::rotate(glm::quat(), 21.f, glm::vec3(1,1,0)))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));
			shiva::core::on("text6/trans", 
				glm::scale(glm::mat4(1.f), glm::vec3(1.3,1.0,1.0))
				*glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::translate(glm::mat4(1.0f), glm::vec3(0.f,0.f,-0.8f))
				*glm::mat4_cast(glm::rotate(glm::quat(), -5.f+sin(d.time*0.2), glm::vec3(0.2,sin(d.time*0.3),cos(d.time*0.4))))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));
			shiva::core::on("text7/trans", 
				glm::scale(glm::mat4(1.f), glm::vec3(1280.0/3,360.0/3,100.0))
				*glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.cam.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.cam.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.cam.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::translate(glm::mat4(1.0f), glm::vec3(6000.f,100.f,-500.f))
				*glm::mat4_cast(glm::rotate(glm::quat(), 11.f, glm::vec3(1,1,0)))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));
			shiva::core::on("text8/trans", 
				glm::scale(glm::mat4(1.f), glm::vec3(1280.0/3,360.0/3,100.0))
				*glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.cam.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.cam.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.cam.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::translate(glm::mat4(1.0f), glm::vec3(7000.f,100.f,-500.f))
				*glm::mat4_cast(glm::rotate(glm::quat(), -21.f, glm::vec3(1,0.2,1)))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));
			shiva::core::on("text9/trans", 
				glm::scale(glm::mat4(1.f), glm::vec3(1280.0/3,360.0/3,100.0))
				*glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.cam.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.cam.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.cam.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::translate(glm::mat4(1.0f), glm::vec3(7000.f,-600.f,-500.f))
				*glm::mat4_cast(glm::rotate(glm::quat(), 21.f, glm::vec3(1,0.3,0.3)))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f))));
		}
		
		d.cam.run(received, d.time, false);
	}
};
behaviour_add(root);