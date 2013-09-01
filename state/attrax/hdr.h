#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"music.h"
#include"shaders.h"
#include"framebuffer.h"
#include"image.h"
#include"camera.h"
using namespace shiva;

class drawpath : public shiva::behaviour {
public:
	struct pathdata{
		bool re;

		bool enable;
		float time;
		std::string timesrc, camerasrc;
		std::map<std::string, float> params;

		VArray va;
		std::map<std::string, Buffer<float>*> data;
		std::map<std::string, Texture*> tx;
		glm::mat4 camera;

		Program pathrenderer;
		std::string frag,geom,vert;
		pathdata(
				std::string frag, std::string geom, std::string vert,
				std::string t, std::string c,
				std::map<std::string, float> iparams = std::map<std::string, float>(),
				std::map<std::string, Buffer<float>*> data = std::map<std::string, Buffer<float>*>(),
				std::map<std::string, Texture*> tx = std::map<std::string, Texture*>()) : params(iparams), data(data), timesrc(t), camerasrc(c), re(false), time(0.f), pathrenderer(frag,geom,vert), frag(frag), geom(geom),vert(vert), tx(tx) {
		}
		void rebuild(){
			re = false;
			pathrenderer.rebuild(frag,geom,vert);
		}
	};
private:
	void init(shiva::state *s, boost::any &data) const {
		auto d = boost::any_cast<pathdata&>(data);
		s->reg("resourcechange");
		s->reg(d.timesrc);
		s->reg(d.camerasrc);
	}

	virtual void uninit(shiva::state *instance, boost::any &data) const {}

	void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<pathdata&>(data);
		if(d.re) d.rebuild();
		if(!d.params.count("on") || d.params["on"] < d.time){
			if(!d.params.count("off") || d.params["off"] > d.time){
				uint minsz = std::numeric_limits<int>::max();
				int i=0;
				std::for_each(d.data.begin(), d.data.end(), [&](std::pair<std::string, Buffer<float>*> p){
					d.va.vertices(d.pathrenderer.getAttribLocation(p.first), p.second->elemsperitem, *p.second, false, 0, 0);
					minsz = std::min(minsz, p.second->size()/p.second->elemsperitem);
					++i;
				});
				if(i>0){
					d.pathrenderer.use();
					std::for_each(d.params.begin(), d.params.end(), [&](std::pair<const std::string, float> p){
						d.pathrenderer.set(p.first,p.second);
					});
					int j = 0;
					std::for_each(d.tx.begin(), d.tx.end(), [&](std::pair<const std::string, Texture*> p){
						d.pathrenderer.bindTexture(p.first,*p.second, j++);
					});
					//Can't draw without inputs
					d.pathrenderer.set("count", (float)minsz);
					d.pathrenderer.set("time", d.time);
					d.pathrenderer.set("camera", d.camera);
					d.va.draw(0, minsz, 1, GL_TRIANGLE_STRIP);
				}
			}
		}
	}
	
	void run(shiva::eventmap &received, shiva::state *s, boost::any &data) const {
		auto &d = boost::any_cast<pathdata&>(data);
		if(received.count("resourcechange")) d.re = true;
		if(received.count(d.timesrc)){
			d.time = boost::any_cast<float>(received[d.timesrc]);
		}
		if(received.count(d.camerasrc)){
			d.camera = boost::any_cast<glm::mat4>(received[d.camerasrc]);
		}
	}
};
behaviour_add(drawpath);

class fssplayer : public shiva::behaviour {
public:
	struct fssdata{
		std::string f;
		FSShader fss;
		bool re;

		bool enable;
		float time;
		std::string timesrc;
		std::map<std::string, float> params;
		std::map<std::string, Texture*> tx;
		glm::mat4 transform;
		fssdata(std::string f, 
				std::string t, 
				std::map<std::string, float> iparams = std::map<std::string, float>(),
				std::map<std::string, Texture*> itx = std::map<std::string, Texture*>(),
				glm::mat4 transform = glm::mat4(1.0f)) : transform(transform), params(iparams), f(f), timesrc(t), fss(f), re(false), time(0.f), tx(itx) {
		}
		void rebuild(){
			re = false;
			fss.rebuild(f);

		}
	};
private:
	void init(shiva::state *s, boost::any &data) const {
		auto d = boost::any_cast<fssdata&>(data);
		s->reg("resourcechange");
		s->reg(d.timesrc);
		s->reg(s->get()+"/trans");
	}

	virtual void uninit(shiva::state *instance, boost::any &data) const {}

	void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<fssdata&>(data);
		if(d.re) d.rebuild();
		if(!d.params.count("on") || d.params["on"] < d.time){
			if(!d.params.count("off") || d.params["off"] > d.time){
				d.fss.clear(false);
				d.fss.set("time", d.time);
				std::for_each(d.params.begin(), d.params.end(), [&](std::pair<const std::string, float> p){
					d.fss.set(p.first,p.second);
				});
				std::for_each(d.tx.begin(), d.tx.end(), [&](std::pair<const std::string, Texture*> p){
					d.fss.input(p.first,*p.second);
				});
				d.fss.transform(d.transform);
				d.fss.display();
			}
		}
	}
	
	void run(shiva::eventmap &received, shiva::state *s, boost::any &data) const {
		auto &d = boost::any_cast<fssdata&>(data);
		if(received.count("resourcechange")) d.re = true;
		if(received.count(d.timesrc)){
			d.time = boost::any_cast<float>(received[d.timesrc]);
		}
		if(received.count(s->get()+"/trans")){
			d.transform = boost::any_cast<glm::mat4>(received[s->get()+"/trans"]);
		}
	}
};
behaviour_add(fssplayer);


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

		rootdata() : fss("statedata/attrax/hdr.frag"), color(1, GL_RGBA16F, GL_TEXTURE_2D), depth(1, GL_DEPTH_COMPONENT32F, GL_TEXTURE_2D), re(false), time(0.f), ptime(-1.f) {
			tx = new std::map<std::string, Texture*>;
		}
		void rebuild(){
			re = false;
			time = ptime = -1.0;
			fss.rebuild("statedata/attrax/hdr.frag");
		}
	};

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
		s->create("music", "musicplayer", std::string("statedata/attrax/fff.mp3"));
		std::map<std::string, float> overlayp;
		PNGImage img2("statedata/attrax/logo2.png");
		(*d.tx)["logo2"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1920, 1080);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, img2.data());
		
		PNGImage beer("statedata/attrax/beer.png");
		(*d.tx)["beer"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1920, 1080);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, beer.data());
		
		
		PNGImage img3("statedata/attrax/ewerk.png");
		(*d.tx)["ewerk"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 1920, 1080);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, img3.data());


		PNGImage gz("statedata/attrax/greetz.png");
		(*d.tx)["greetz"] = new Texture(1, GL_RGBA, GL_TEXTURE_2D, 8640, 89);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8640, 89, 0, GL_RGBA, GL_UNSIGNED_BYTE, gz.data());
		
		(*d.tx)["wform"] = new Texture(1, GL_R16F, GL_TEXTURE_2D, 1024, 1);
		(*d.tx)["spectrum"] = new Texture(1, GL_R16F, GL_TEXTURE_2D, 1024, 1);
		std::map<std::string, Buffer<float>*> path;
		

		
		overlayp["on"] = 0.0;
		overlayp["off"] = 20.0;
		s->create("background", "fssplayer", fssplayer::fssdata(std::string("statedata/attrax/background.frag"), std::string("music/time"), overlayp));
		s->create("road", "fssplayer", fssplayer::fssdata(std::string("statedata/attrax/road.frag"), std::string("music/time"), overlayp));
		overlayp["on"] = 5.0;

		s->create("overlay", "fssplayer", fssplayer::fssdata(std::string("statedata/attrax/overlay.frag"), std::string("music/time"), overlayp, *d.tx));

		overlayp["on"] = 19.9;
		overlayp["off"] = 20.1;
		overlayp["fadetime"] = 0.1;
		s->create("whiteout1", "fssplayer", fssplayer::fssdata(std::string("statedata/attrax/whiteout.frag"), std::string("music/time"), overlayp));
		
		overlayp["on"] = 21.5;
		overlayp["off"] = 90.0;
		s->create("background2", "fssplayer", fssplayer::fssdata(std::string("statedata/attrax/background2.frag"), std::string("music/time"), overlayp));
		overlayp["off"] = 40.0;
		
		path["position"] = new Buffer<float>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 3);
		std::vector<float> f;
		for(int i=0;i<10000;++i){
			f.push_back(-0.6);
			f.push_back(-0.6);
			f.push_back(i*5.0);

			f.push_back(0.6);
			f.push_back(-0.6);
			f.push_back(i*5.0);
		}
		path["position"]->data(f);
		
		overlayp["offset"] = 20.0;
		s->create("pathleft", "drawpath", drawpath::pathdata("statedata/attrax/path1.frag","statedata/attrax/path1.geom","statedata/attrax/path1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = -20.0;
		s->create("pathright", "drawpath", drawpath::pathdata("statedata/attrax/path1.frag","statedata/attrax/path1.geom","statedata/attrax/path1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 0.0;
		overlayp["blink"] = 1.0;
		s->create("pathcenter", "drawpath", drawpath::pathdata("statedata/attrax/path1.frag","statedata/attrax/path1.geom","statedata/attrax/path1.vert",std::string("music/time"), std::string("camera"), overlayp, path));

		overlayp["offset"] = 25.0;
		overlayp["blink"] = 0.0;
		overlayp["mark"] = 1.0;
		s->create("pathlights1", "drawpath", drawpath::pathdata("statedata/attrax/path1.frag","statedata/attrax/path1.geom","statedata/attrax/path1.vert",std::string("music/time"), std::string("camera"), overlayp, path));

		overlayp["offset"] = -25.0;
		overlayp["blink"] = 0.0;
		overlayp["mark"] = 1.0;
		s->create("pathlights2", "drawpath", drawpath::pathdata("statedata/attrax/path1.frag","statedata/attrax/path1.geom","statedata/attrax/path1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		
		overlayp["mark"] = 0.0;
		overlayp["offset2"] = 1.0;
		overlayp["offset"] = 20.0;
		s->create("pathleft2", "drawpath", drawpath::pathdata("statedata/attrax/path1.frag","statedata/attrax/path1.geom","statedata/attrax/path1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = -20.0;
		s->create("pathright2", "drawpath", drawpath::pathdata("statedata/attrax/path1.frag","statedata/attrax/path1.geom","statedata/attrax/path1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 0.0;
		overlayp["blink"] = 1.0;
		s->create("pathcenter2", "drawpath", drawpath::pathdata("statedata/attrax/path1.frag","statedata/attrax/path1.geom","statedata/attrax/path1.vert",std::string("music/time"), std::string("camera"), overlayp, path));

		overlayp["offset"] = 25.0;
		overlayp["blink"] = 0.0;
		overlayp["mark"] = 1.0;
		s->create("pathlights12", "drawpath", drawpath::pathdata("statedata/attrax/path1.frag","statedata/attrax/path1.geom","statedata/attrax/path1.vert",std::string("music/time"), std::string("camera"), overlayp, path));

		overlayp["offset"] = -25.0;
		overlayp["blink"] = 0.0;
		overlayp["mark"] = 1.0;
		s->create("pathlights22", "drawpath", drawpath::pathdata("statedata/attrax/path1.frag","statedata/attrax/path1.geom","statedata/attrax/path1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		

		overlayp["on"] = 21.5;
		overlayp["off"] = 60.0;
		overlayp["slomoon"] = 35.0;
		overlayp["slomooff"] = 45.0;
		overlayp["offset"] = 0.0;
		s->create("cubes1", "drawpath", drawpath::pathdata("statedata/attrax/cube1.frag","statedata/attrax/cube1.geom","statedata/attrax/cube1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 1.0;
		s->create("cubes2", "drawpath", drawpath::pathdata("statedata/attrax/cube1.frag","statedata/attrax/cube1.geom","statedata/attrax/cube1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 2.0;
		s->create("cubes3", "drawpath", drawpath::pathdata("statedata/attrax/cube1.frag","statedata/attrax/cube1.geom","statedata/attrax/cube1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		overlayp["offset"] = 3.0;
		s->create("cubes4", "drawpath", drawpath::pathdata("statedata/attrax/cube1.frag","statedata/attrax/cube1.geom","statedata/attrax/cube1.vert",std::string("music/time"), std::string("camera"), overlayp, path));
		

		overlayp["on"] = 45.0;
		
		overlayp["off"] = 60.0;
		overlayp["mark"] = 0;

		overlayp["offset"] = 25.0;
		s->create("2pathlights1", "drawpath", drawpath::pathdata("statedata/attrax/path2.frag","statedata/attrax/path2.geom","statedata/attrax/path2.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));

		overlayp["offset"] = -25.0;
		s->create("2pathlights2", "drawpath", drawpath::pathdata("statedata/attrax/path2.frag","statedata/attrax/path2.geom","statedata/attrax/path2.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));
		
		overlayp["on"] = 45;
		overlayp["off"] = 60;
		s->create("greetz", "fssplayer", fssplayer::fssdata(std::string("statedata/attrax/overlay2.frag"), std::string("music/time"), overlayp, *d.tx));

		
		
		overlayp["on"] = 60;
		overlayp["off"] = 75;

		overlayp["mark"] = 0.0;
		overlayp["offset2"] = 0.0;
		overlayp["offset"] = 20.0;
		s->create("3pathleft", "drawpath", drawpath::pathdata("statedata/attrax/path3.frag","statedata/attrax/path3.geom","statedata/attrax/path3.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));
		overlayp["offset"] = -20.0;
		s->create("3pathright", "drawpath", drawpath::pathdata("statedata/attrax/path3.frag","statedata/attrax/path3.geom","statedata/attrax/path3.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));
		overlayp["offset"] = 0.0;
		overlayp["blink"] = 1.0;
		s->create("3pathcenter", "drawpath", drawpath::pathdata("statedata/attrax/path3.frag","statedata/attrax/path3.geom","statedata/attrax/path3.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));

		overlayp["offset"] = 25.0;
		overlayp["blink"] = 0.0;
		overlayp["mark"] = 1.0;
		s->create("3pathlights1", "drawpath", drawpath::pathdata("statedata/attrax/path3.frag","statedata/attrax/path3.geom","statedata/attrax/path3.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));

		overlayp["offset"] = -25.0;
		overlayp["blink"] = 0.0;
		overlayp["mark"] = 1.0;
		s->create("3pathlights2", "drawpath", drawpath::pathdata("statedata/attrax/path3.frag","statedata/attrax/path3.geom","statedata/attrax/path3.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));
		
		overlayp["mark"] = 0.0;
		overlayp["offset2"] = 1.0;
		overlayp["offset"] = 20.0;
		s->create("3pathleft2", "drawpath", drawpath::pathdata("statedata/attrax/path3.frag","statedata/attrax/path3.geom","statedata/attrax/path3.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));
		overlayp["offset"] = -20.0;
		s->create("3pathright2", "drawpath", drawpath::pathdata("statedata/attrax/path3.frag","statedata/attrax/path3.geom","statedata/attrax/path3.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));
		overlayp["offset"] = 0.0;
		overlayp["blink"] = 1.0;
		s->create("3pathcenter2", "drawpath", drawpath::pathdata("statedata/attrax/path3.frag","statedata/attrax/path3.geom","statedata/attrax/path3.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));

		overlayp["offset"] = 25.0;
		overlayp["blink"] = 0.0;
		overlayp["mark"] = 1.0;
		s->create("3pathlights12", "drawpath", drawpath::pathdata("statedata/attrax/path3.frag","statedata/attrax/path3.geom","statedata/attrax/path3.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));

		overlayp["offset"] = -25.0;
		overlayp["blink"] = 0.0;
		overlayp["mark"] = 1.0;
		s->create("3pathlights22", "drawpath", drawpath::pathdata("statedata/attrax/path3.frag","statedata/attrax/path3.geom","statedata/attrax/path3.vert",std::string("music/time"), std::string("camera"), overlayp, path, *d.tx));
		
		overlayp["on"] = 70.0;
		overlayp["off"] = 110.0;
		overlayp["offset"] = 0.0;
		s->create("2cubes1", "drawpath", drawpath::pathdata("statedata/attrax/cube2.frag","statedata/attrax/cube2.geom","statedata/attrax/cube2.vert",std::string("music/time"), std::string("camera2"), overlayp, path, *d.tx));
		overlayp["offset"] = 1.0;
		s->create("2cubes2", "drawpath", drawpath::pathdata("statedata/attrax/cube2.frag","statedata/attrax/cube2.geom","statedata/attrax/cube2.vert",std::string("music/time"), std::string("camera2"), overlayp, path, *d.tx));
		overlayp["offset"] = 2.0;
		s->create("2cubes3", "drawpath", drawpath::pathdata("statedata/attrax/cube2.frag","statedata/attrax/cube2.geom","statedata/attrax/cube2.vert",std::string("music/time"), std::string("camera2"), overlayp, path, *d.tx));
		overlayp["offset"] = 3.0;
		s->create("2cubes4", "drawpath", drawpath::pathdata("statedata/attrax/cube2.frag","statedata/attrax/cube2.geom","statedata/attrax/cube2.vert",std::string("music/time"), std::string("camera2"), overlayp, path, *d.tx));
		

		
		overlayp["on"] = 85.0;
		overlayp["off"] = 2000.0;
		s->create("background3", "fssplayer", fssplayer::fssdata(std::string("statedata/attrax/background3.frag"), std::string("music/time"), overlayp, *d.tx));
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
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			
			std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();}); 
			glDisable(GL_BLEND);
			
		});
		d.fss.set("time", d.time);
		d.fss.program().use();
		d.fss.program().bindTexture("color", d.color);
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
			d.cam.init(instance, "statedata/attrax/camera.0");
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

			shiva::core::on("greetz/trans", 
				
				glm::scale(glm::mat4(1.f), glm::vec3(3000.0,100.0,100.0))
				
				
				*glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f, 5000.0f)
				
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.cam.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.cam.rot.x, glm::vec3(0,1,0)))
			
				*glm::translate(glm::mat4(1.0f), d.cam.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				
				*glm::translate(glm::mat4(1.0f), glm::vec3(0.f,100.f,-6500.f))
				*glm::mat4_cast(glm::rotate(glm::quat(), -90.f, glm::vec3(0,1,0)))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f)))
				);
		}
		
		d.cam.run(received, d.time, false);
	}
};
behaviour_add(root);