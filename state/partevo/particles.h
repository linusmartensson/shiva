#include"state.h"
#include"shaders.h"
#include"clengine.h"
#include"resourcetracker.h"
#include"framebuffer.h"
#include"music.h"
#include<algorithm>
#include<fstream>
#include"fonts.h"

struct cpos{
	glm::vec3 trans;
	glm::vec2 rot;
	float time;

	cpos operator+(const cpos &rhs) const {
		cpos c;
		c.trans = trans+rhs.trans;
		c.rot = rot+rhs.rot;
		return c;
	}
	cpos operator*(const float f) const {
		cpos c = *this;
		c.trans *= f;
		c.rot *= f;
		return c;
	}
	cpos operator-(const cpos &rhs) const {
		return *this+rhs*-1.f;
	}
	cpos operator/(const float f) const {
		return *this*(1.f/f);
	}

	cpos() : trans(0.f), rot(0.f), time(0.f) {}

	cpos(glm::vec3 trans, glm::vec3 rot) : trans(trans), rot(rot), time(0.f) {}
	virtual ~cpos(){}
};
std::istream& operator>>(std::istream &iss, cpos &v){
	iss>>v.trans.x;
	iss>>v.trans.y;
	iss>>v.trans.z;
	iss>>v.rot.x;
	iss>>v.rot.y;
	iss>>v.time;
	return iss;
}
std::ostream& operator<<(std::ostream &oss, const cpos &v){
	oss<<v.trans.x<<" ";
	oss<<v.trans.y<<" ";
	oss<<v.trans.z<<" ";
	oss<<v.rot.x<<" ";
	oss<<v.rot.y<<" ";
	oss<<v.time<<"\n";
	return oss;
}
cpos hermiteInterpolate(cpos y0, cpos y1,cpos y2, cpos y3,float mu,float tension,float bias) {
	cpos m0,m1;
	float mu2,mu3;
	float a0, a1, a2, a3;
	mu2 = mu*mu;
	mu3 = mu2*mu;
	tension = (1.f-tension)/2.f;
	m0 = (y1-y0)*(1.f+bias)*tension;
	m0 = m0+(y2-y1)*(1.f-bias)*tension;
	m1 = (y2-y1)*(1.f+bias)*tension;
	m1 = m1+(y3-y2)*(1.f-bias)*tension;
	a0 = mu3*2.f - 3.f*mu2 + 1.f;
	a1 = mu3 - 2.f*mu2 + mu;
	a2 = mu3 - mu2;
	a3 = mu3*-2.f + mu2*3.f;

	return y1*a0 + m0*a1 + m1*a2 + y2*a3;
}
cpos lerp(cpos y1, cpos y2, float m){
	return y1*(1.f-m)+y2*m;
}
struct camera : public cpos {
	
	glm::vec3 prevtrans;
	std::vector<cpos> c;
	bool automate;
	std::string name;
	glm::vec2 prevf;
	bool left,right,up,down;

	camera() : automate(true), left(false), right(false), up(false), down(false) {
		prevf.x = shiva::core::width()/2.f;
			prevf.y = shiva::core::height()/2.f;
	}

	void run(shiva::eventmap &r, float t, bool paused){
		time = t;
		prevtrans = trans;
		if(r.count("kinput")){

			if(shiva::core::down('Q')){
				automate = !automate;
			}
			
			if(automate) return;
			
			if(shiva::core::down('E')){
				size_t i=0;
				/*for(;i<c.size() && c[i].time<=time;++i); 
				c.insert(c.begin()+i, cpos(*this));*/
				Log::info()<<cpos(*this);
			}
			if(shiva::core::down('L')){
				rebuild();
			}
			if(shiva::core::down('1')){
				if(name != ""){
					std::ofstream off(name);
					for(size_t i=0;i<c.size();++i){
						off<<c[i];
					}
				}
			}

			if(shiva::core::down('W')){
				up = true;
			} else {
				up = false;
			}
			if(shiva::core::down('S')){
				down = true;
			} else {
				down = false;
			}
			if(shiva::core::down('A')){
				left = true;
			} else left = false;

			if(shiva::core::down('D')){
				right = true;
			} else right = false;
			
		}
		if(r.count("minput")){
			std::pair<int,int> f = boost::any_cast<std::pair<int,int>>(r["minput"]);

			

			if(automate) return;
			rot.x -= (f.first-prevf.x)*0.1f;
			rot.y -= (f.second-prevf.y)*0.1f;

			prevf = glm::vec2(f.first,f.second);
		}

		if(automate){
			cpos s = *this, d = *this, ss = *this, dd = *this;
			size_t i=0;
			for(;i<=c.size();++i){
				if(i<c.size() && c[i].time < time) continue;
				
				if(i-2 >= 0 && c.size()>0) ss = c[i-2];
				if(i-1 >= 0 && c.size()>0) s = c[i-1];
				if(i < c.size()) d = c[i];
				if(i+1 < c.size()) dd = c[i+1];

				break;
			}
			if(i < c.size()) {
				float dds = d.time-s.time;
				float i = ((d.time-time)/dds);

				trans = s.trans*i+d.trans*(1.f-i);
				rot = s.rot*i+d.rot*(1.f-i);

				cpos c = hermiteInterpolate(ss,s,d,dd, 1.f-i,0.f,0.f);
				trans = c.trans;
				rot = c.rot;
			}

		} else {
			
			trans = trans + 
			(glm::vec4(((left?1.0:0.0)-(right?1.0:0.0))*-100.f,0.f,((up?1.0:0.0)-(down?1.0:0.0))*100.f,1.0f)*
			glm::mat4_cast(
				glm::rotate(glm::quat(), rot.y, glm::vec3(1,0,0))*
				glm::rotate(glm::quat(), rot.x, glm::vec3(0,1,0)))
			).swizzle(glm::X, glm::Y, glm::Z);
		}
			
		/*rot.y = 30.0;
		float de = (float)time*0.4f;
		rot.x = de*100*9.f/16.f-70;
		trans.z = -5000*sin(de);
		trans.x = -5000*cos(de);
		trans.y = -300.f;*/
	}
	void rebuild(){
		c.clear();
		trans=glm::vec3(0.0);
		rot=glm::vec2(0.0);
		automate = false;
		if(name!=""){
			this->name = name;
			std::ifstream iff(name);
			while(iff.good()){
				cpos cc;
				iff>>cc;
				if(iff.bad()||iff.eof()||iff.fail())break;
				c.push_back(cc);
				automate = true;
			}
		}
		if(c.size() == 0){
			c.push_back(cpos(*this));
			c.push_back(cpos(*this));
		}
	}
	void init(shiva::state *s, std::string name=""){
		
		s->reg("kinput");
		s->reg("minput");

		if(name != ""){
			name = "resources/"+name;
		}
		this->name = name;
		rebuild();
	}
};
struct startdata{
	int pointcount;
	FrameBuffer fb;
	FSShader fss;
	FSShader fss2;
	FSShader tofss;
	Program p1,p2,p3,p4,p5,p6,p7,p8,p9;
	CLProgram *clparticles, *clline;

	float last;
	std::string clp, cll;
	float aspectRatio;
	Texture t;
	Texture t1;
	Texture t2;
	Texture t3;

	Texture text0;
	Texture text1;
	Texture text2;
	Texture text3;
	Texture text4;
	Texture text5;
	Texture text6;
	Texture text7;
	Texture text8;
	FrameBuffer tfb;
	
	Buffer<GLfloat> pos,acc,state,astate,color,screenposition,oldscreenposition,pcam,pos2,temp;
	std::vector<float> times, texts;
	uint timespos, textspos;
	VArray va,va2;
	bool reload;
	bool paused;
	int count;
	float time;
	float startTime;
	float prevTime;
	camera c;
	music *m;
	font *f;

	void rebuild(){
		  clparticles->rebuild(loadFile(clp, true));
		  clline->rebuild(loadFile(cll, true));
		  p1.rebuild("statedata/de/minimal1.frag","statedata/de/minimal1.geom","statedata/de/minimal1.vert")
		, p2.rebuild("statedata/de/minimal2.frag","statedata/de/minimal2.geom","statedata/de/minimal2.vert")
		, p4.rebuild("statedata/de/minimal4.frag","statedata/de/minimal4.geom","statedata/de/minimal4.vert")
		, p3.rebuild("statedata/de/minimal3.frag","statedata/de/minimal3.geom","statedata/de/minimal3.vert")
		, p5.rebuild("statedata/de/minimal5.frag","statedata/de/minimal5.geom","statedata/de/minimal5.vert")
		, p6.rebuild("statedata/de/minimal6.frag","statedata/de/minimal6.geom","statedata/de/minimal6.vert")
		, p7.rebuild("statedata/de/minimal7.frag","statedata/de/minimal7.geom","statedata/de/minimal7.vert")
		, p8.rebuild("statedata/de/minimal8.frag","statedata/de/minimal8.geom","statedata/de/minimal8.vert")
		, p9.rebuild("statedata/de/minimal9.frag","statedata/de/minimal9.geom","statedata/de/minimal9.vert")
		;
		  c.rebuild();
		  fss2.rebuild("statedata/de/test.frag");
		  fss.rebuild("statedata/de/output.frag");
		  tofss.rebuild("statedata/de/text.frag", "statedata/de/text.geom");
		  fb.colorbuffer(0, t);
		  fb.colorbuffer(1, t1);
		  fb.colorbuffer(2, t2);
		  fb.depthbuffer(t3);
		  
		  if(f!=0) delete f;
		  f = new font("resources/fonts/TECHNOID.ttf", 32);
		  
		  m->time(0);
		  last = 0.f;
		  count = 0;
	}

	startdata() : reload(false), pointcount(40000), fss2("statedata/de/test.frag"), fss("statedata/de/output.frag"), timespos(0)
	,p1("statedata/de/minimal1.frag","statedata/de/minimal1.geom","statedata/de/minimal1.vert")
	,p2("statedata/de/minimal2.frag","statedata/de/minimal2.geom","statedata/de/minimal2.vert")
	,p4("statedata/de/minimal4.frag","statedata/de/minimal4.geom","statedata/de/minimal4.vert")
	,p5("statedata/de/minimal5.frag","statedata/de/minimal5.geom","statedata/de/minimal5.vert")
	,p6("statedata/de/minimal6.frag","statedata/de/minimal6.geom","statedata/de/minimal6.vert")
	,p7("statedata/de/minimal7.frag","statedata/de/minimal7.geom","statedata/de/minimal7.vert")
	,p8("statedata/de/minimal8.frag","statedata/de/minimal8.geom","statedata/de/minimal8.vert")
	,p9("statedata/de/minimal9.frag","statedata/de/minimal9.geom","statedata/de/minimal9.vert")
	,p3("statedata/de/minimal3.frag","statedata/de/minimal3.geom","statedata/de/minimal3.vert")
	,count(0)
	,paused(false), m(new music("statedata/de/Malmen_-_Signal_Void.mp3")), 
	t(1, GL_RGBA16F, GL_TEXTURE_2D), 
	t1(1, GL_RGBA16F, GL_TEXTURE_2D) , 
	t2(1, GL_RGBA16F, GL_TEXTURE_2D), f(0),
	text0(1, GL_RGBA16F, GL_TEXTURE_2D),
	text1(1, GL_RGBA16F, GL_TEXTURE_2D),
	text2(1, GL_RGBA16F, GL_TEXTURE_2D),
	text3(1, GL_RGBA16F, GL_TEXTURE_2D),
	text4(1, GL_RGBA16F, GL_TEXTURE_2D),
	text5(1, GL_RGBA16F, GL_TEXTURE_2D),
	text6(1, GL_RGBA16F, GL_TEXTURE_2D),
	text7(1, GL_RGBA16F, GL_TEXTURE_2D),
	text8(1, GL_RGBA16F, GL_TEXTURE_2D),
	t3(1, GL_DEPTH_COMPONENT32, GL_TEXTURE_2D),
	tofss("statedata/de/text.frag", "statedata/de/text.geom"),
	last(0)
	{
		clp = "builtins/tools.cl statedata/de/particleengine.cl";
		cll = "builtins/tools.cl statedata/de/clline.cl";
		clparticles = new CLProgram(loadFile(clp));
		clline = new CLProgram(loadFile(cll));
		fb.colorbuffer(0, t);
		fb.colorbuffer(1, t1);
		fb.colorbuffer(2, t2);
		fb.depthbuffer(t3);
		f = new font("resources/fonts/TECHNOID.ttf", 32);
	}
	void destroy(){
		delete clparticles;
		delete clline;
		delete m;
		delete f;
	}
};
class particles : public shiva::behaviour {
	virtual void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		startdata &d = (boost::any_cast<startdata&>(data));
		bool move = false;
		if(received.count("resourcechange")){
			d.reload = true;
		}
		if(received.count("kinput")){
			std::pair<int,bool> g = boost::any_cast<std::pair<int,bool>>(received["kinput"]);
			if(g.first == GLFW_KEY_ESC)
				shiva::core::stop();
			if(g.first == GLFW_KEY_RIGHT){
				d.startTime -= 1.0;
				move = true;
			}
			if(g.first == GLFW_KEY_LEFT){
				d.startTime += 1.0;
				move = true;
			}
			if(g.first == GLFW_KEY_SPACE && g.second == true){
				d.paused = !d.paused;
				if(d.paused == false){
					d.startTime += (float)glfwGetTime()-d.prevTime;
					d.m->pause(false);
				} else {
					d.prevTime = (float)glfwGetTime();
					d.m->pause(true);
				}
			}
		}
		if(!d.paused){
			d.time = (float)glfwGetTime()-d.startTime;
		} else {
			d.time = d.prevTime-d.startTime;
			move = true;
		}
		if(move)
			d.m->time(d.time);
		if(d.time < 0){
			d.startTime += d.time;
		}
		
		d.aspectRatio = 16.f/9.f;
		
		d.c.run(received, d.time, d.paused);
	}
	virtual void render(shiva::childvec children, boost::any &data) const {
		glDisable(GL_BLEND);
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE,GL_ZERO);

		startdata &t = (boost::any_cast<startdata&>(data));
		if(t.reload) reload(data);

		startdata &d = (boost::any_cast<startdata&>(data));
		float speed = 0.1;
		glm::mat4 cam = glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.c.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.c.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f)));
		glm::mat4 rotcam = glm::transpose(glm::mat4_cast(glm::rotate(glm::quat(), d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), d.c.rot.x, glm::vec3(0,1,0))));
		glm::vec4 ptrans(d.c.trans, 0.0); 
		
		if(d.times[d.timespos] < d.time){
			d.timespos++;
			d.clline->set("clzero", 1, 0.f);
			d.clline->set("clzero", 2, 0.f);
			d.clline->set("clzero", 3, 0.f);
			d.clline->set("clzero", 4, 0.f);
//			d.clline->set("clzero", 0, d.astate);
//			d.clline->run("clzero", d.pointcount);
			
			d.clline->set("clboom", 0, d.astate);
			d.clline->set("clboom", 1, 342.f);
			d.clline->set("clboom", 2, (float)rand());
			d.clline->set("clboom", 3, 0.5f);

			d.clline->run("clboom", d.pointcount);
		}
		
		if(d.timespos>0 && d.times[d.timespos-1] > d.time){
			d.timespos--;
		}

		if(d.timespos >= d.times.size()) shiva::core::stop();
		float toff = d.timespos>0?(d.time - d.times[d.timespos-1]):d.time;
		int active = d.timespos;//int(d.time/10)%4;
		
		d.clline->set("clplane", 1, toff);
		d.clline->set("clline", 1, toff);
		d.clline->set("clline2", 1, toff);
		d.clline->set("clconway", 1, toff);
		d.clline->set("clconwaydist", 1, toff);
		
		d.clline->set("cldgrow", 1, toff);
		
		d.fss2.set("rot", glm::mat3(rotcam));
		d.fss2.set("trans", glm::vec3(ptrans*speed));
		d.fss2.set("time", d.time);
		d.fss2.program().set("camera", cam);
		
		for(int i=0;i<9;++i){
			switch(i){
			case 0:
				d.tfb.colorbuffer(0, d.text0);
				break;
			case 1:
				d.tfb.colorbuffer(0, d.text1);
				break;
			case 2:
				d.tfb.colorbuffer(0, d.text2);
				break;
			case 3:
				d.tfb.colorbuffer(0, d.text3);
				break;
			case 4:
				d.tfb.colorbuffer(0, d.text4);
				break;
			case 5:
				d.tfb.colorbuffer(0, d.text5);
				break;
			case 6:
				d.tfb.colorbuffer(0, d.text6);
				break;
			case 7:
				d.tfb.colorbuffer(0, d.text7);
				break;

			case 8:
				d.tfb.colorbuffer(0, d.text8);
				break;
			}
			d.tfb.render([&]{
				glDisable(GL_BLEND);
				glClearColor(0,0,0,0);
				glDisable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
				d.f->setDisplaySize(shiva::core::width(), shiva::core::height());
				d.f->setPenPosition(0,2);
				d.f->setPenColor(1.0,1.0,1.0);
			
				
				//0x5
				switch(i){
				break; case 0:
					d.f->draw("0x5");
				break; case 1:
					d.f->draw("code&gfx:Linus music:Malmen");
				break; case 2:
						//1953.67 2144.53 -20488.5 -1133.71 -9.58445 129.773
					d.f->draw("malmen fsfreak w0lfis joule");

				break; case 3:
						//29674.4 10765.1 1577.45 -964.412 -23.6848 141.094
									d.f->draw("topatisen xiny cryptic w&c");

				break; case 4:
						//-2100.55 5547.96 -3411.34 -790.807 -57.0453 190.772
									d.f->draw("wiklund p-rabbit smash sajko");

				break; case 5:
						//882.816 921.803 9352.32 -836.104 -21.8553 211.871
									d.f->draw("zalza lusius kalaspuff michu");

				break; case 6:
						//3832.99 -1394.19 20358.1 -784.96 -3.80803 228.727
					d.f->draw("titan chippojkarna facerape");
				break; case 7:
					//8673.74 -1892.67 29764.6 -942.307 -9.13882 234.74
					d.f->draw("fairlight mental razor1911 ffx");
				break; case 8:
					//15314 -2400.39 22812.7 -920.169 -5.35336 250.527
					d.f->draw("approximate candela even tbl");
				}
			});
		}
		//active = 4;
		d.fb.render([&]{
			glDisable(GL_BLEND);
			glClearColor(0,0,0,1);
			glClear(GL_DEPTH_BUFFER_BIT);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
			d.fss2.set("skip", 1.f);
			d.fss2.display();
			d.fss2.set("skip", 0.f);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

						
		switch(active){
			case 0:
				d.clline->run("clplane", d.pointcount);
				
				d.p5.bindAttrib(0, "position");
				d.p5.use();
				d.p5.set("camera", cam);
				d.p5.set("time", toff);
				d.va2.draw(0,d.pointcount,1,GL_POINTS);
				break;
			case 1:
				d.clline->run("clconway", d.pointcount);
				
				d.p4.bindAttrib(0, "position");
				d.p4.bindAttrib(1, "astate");
				d.p4.use();
				d.p4.set("camera", cam);
				d.p4.set("time", d.time);
				d.va2.draw(0,d.pointcount,1,GL_POINTS);
				if(d.time-d.last > 0.1){
					d.last = d.time;
					d.clline->run("doastate", d.pointcount);
				}
				break;
			case 2:
				d.clline->run("clline", d.pointcount);
				d.p3.bindAttrib(0, "position");
				d.p3.use();
				d.p3.set("camera", cam);
				d.p3.set("time", toff);
				d.va2.draw(0,d.pointcount,1,GL_TRIANGLE_STRIP);
				break;
			case 3:
				d.clline->run("cldgrow", d.pointcount);
				d.p6.bindAttrib(0, "position");
				d.p6.bindAttrib(1, "astate");
				d.p6.use();
				d.p6.set("camera", cam);
				d.p6.set("time", d.time);
				d.p6.set("inv", (float)((d.count/40)%2));
				d.va2.draw(0,d.pointcount,1,GL_POINTS);
				if(d.time-d.last > 0.2){
					d.last = d.time;
					
					d.clline->run("doagrow", d.pointcount);
					d.count++;
				}
				if(d.count%40==0){
					d.clline->set("clboom", 0, d.astate);
					d.clline->set("clboom", 1, toff);
					d.clline->set("clboom", 2, (float)rand());
					d.clline->set("clboom", 3, 0.9f);

					d.clline->run("clboom", d.pointcount);
				}
				break;
			case 4:
				d.fss2.display();
				d.clline->run("clline2", d.pointcount);
				d.p9.bindAttrib(0, "position");
				d.p9.use();
				d.p9.set("camera", cam);
				d.p9.set("time", toff);
				d.va2.draw(0,d.pointcount,1,GL_TRIANGLE_STRIP);
				break;
			case 5:
				d.clline->run("clconwaydist", d.pointcount);
				
				d.p4.bindAttrib(0, "position");
				d.p4.bindAttrib(1, "astate");
				d.p4.use();
				d.p4.set("camera", cam);
				d.p4.set("time", d.time);
				d.va2.draw(0,d.pointcount,1,GL_POINTS);
				if(d.time-d.last > 0.1){
					d.last = d.time;
					d.clline->run("doastate", d.pointcount);
				}
				break;
			}
		});
		
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.f,0.f,0.f,0.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);


		d.pcam.data(16, glm::value_ptr(cam));
		
		d.clparticles->set("particleengine", 9, d.paused?1.f:0.f);
		d.clparticles->set("particleengine", 5, toff);
		d.clparticles->set("particleengine", 6, d.pcam);
		d.clparticles->run("particleengine", d.pointcount);
		
		d.p2.bindAttrib(0, "position");
		d.p2.bindAttrib(1, "position2");
		d.p2.bindAttrib(2, "color");
		d.p1.bindAttrib(0, "position");
		d.p1.bindAttrib(1, "position2");
		d.p1.bindAttrib(2, "color");
		
		switch(active){
		case 0:
		case 1:
		d.p1.use();
		d.va.draw(0,d.pointcount,1,GL_POINTS);
			break;

		default:
		d.p1.use();
		d.va.draw(d.pointcount/2,d.pointcount/2-1,1,GL_POINTS);
		d.p2.use();
		d.va.draw(0,d.pointcount/2,1,GL_POINTS);
		}

		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		d.fss.set("time", toff);
		switch(active){
		case 0:
			d.fss.set("vis", clamp(2.f-toff/20.f,0.f,1.f));
			d.fss.set("range", toff*100.f-250.f);
			break;
		case 1:
			d.fss.set("vis", clamp(2.f-toff/10.f,0.f,0.8f));
			d.fss.set("range", 5000.f);
			break;
		case 2:
			d.fss.set("vis", clamp(2.f-toff/20.f,0.f,0.5f));
			d.fss.set("range", 50000.f);
			break;
		case 4:
			glBlendFunc(GL_ONE,GL_ONE);
			d.fss.set("vis", 1.0f);
			d.fss.set("range", 50000.f);
			break;
		default:
			d.fss.set("vis", 0.5f);
			d.fss.set("range", 5000.f);
		}
		d.fss.program().bindTexture("w", d.t, 0);
		d.fss.program().bindTexture("a", d.t1, 1);
		d.fss.program().bindTexture("c", d.t2, 2);
		d.fss.clear(false);
		d.fss.display();
		
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		d.tofss.clear(false);
		switch(active){
		case 0:
		if(d.time > 28){
			d.tofss.input("tex", d.text0);
			d.tofss.transform(glm::transpose(glm::translate(glm::transpose(cam),glm::vec3(675.351f, -1510.37f, -12826.8f)*speed)));
			d.tofss.display();
			d.tofss.input("tex", d.text1);
			d.tofss.transform(glm::transpose(glm::translate(glm::transpose(cam),glm::vec3(2145.9f, -1399.1f, -15231.3f)*speed)));
			d.tofss.display();
		}
		break;
		case 2:
			if(d.time < 121.0) break;
		//1953.67 2144.53 -20488.5 -1133.71 -9.58445 129.773
		d.tofss.input("tex", d.text2);
		d.tofss.transform(glm::transpose(glm::translate(glm::transpose(cam),glm::vec3(1953.67f, 2144.53f, -20488.5f)*speed)));
		d.tofss.display();

			//29674.4 10765.1 1577.45 -964.412 -23.6848 141.094
		d.tofss.input("tex", d.text3);
		d.tofss.transform(glm::transpose(glm::translate(glm::transpose(cam),glm::vec3(29674.4f, 10765.1f, 1577.45f)*speed)));
		d.tofss.display();
			//-2100.55 5547.96 -3411.34 -790.807 -57.0453 190.772
		d.tofss.input("tex", d.text4);
		d.tofss.transform(glm::transpose(glm::translate(glm::transpose(cam),glm::vec3(-2100.55f, 5547.96f, -3411.34f)*speed)));
		d.tofss.display();

			//882.816 921.803 9352.32 -836.104 -21.8553 211.871
		d.tofss.input("tex", d.text5);
		d.tofss.transform(glm::transpose(glm::translate(glm::transpose(cam),glm::vec3(882.816f, 921.803f, 9352.32f)*speed)));
		d.tofss.display();

			//3832.99 -1394.19 20358.1 -784.96 -3.80803 228.727
		d.tofss.input("tex", d.text6);
		d.tofss.transform(glm::transpose(glm::translate(glm::transpose(cam),glm::vec3(3832.99f, -1394.19f, 20358.1f)*speed)));
		d.tofss.display();
		//8673.74 -1892.67 29764.6 -942.307 -9.13882 234.74
		d.tofss.input("tex", d.text7);
		d.tofss.transform(glm::transpose(glm::translate(glm::transpose(cam),glm::vec3(8673.74f, -1892.67f, 29764.6f)*speed)));
		d.tofss.display();
		//15314 -2400.39 22812.7 -920.169 -5.35336 250.527
		d.tofss.input("tex", d.text8);
		d.tofss.transform(glm::transpose(glm::translate(glm::transpose(cam),glm::vec3(15314.f, -2400.39f, 22812.7f)*speed)));
		d.tofss.display();
		}
	}
	void reload(boost::any &data) const {
		auto &d = boost::any_cast<startdata&>(data);
		
		d.rebuild();
		d.timespos = 0;
		d.times.clear();
		d.times.push_back(49.58f);
		d.times.push_back(94.6828f);
		d.times.push_back(185.133);
		d.times.push_back(217.963);
		d.times.push_back(327.133);
		
		d.startTime = (float)glfwGetTime();
		d.time = (float)glfwGetTime()-d.startTime;
		d.aspectRatio = 16.f/9.f;
		
		float de = (float)d.time*0.4f;

		std::vector<GLfloat> glf;
		for(int i=0;i<4*d.pointcount;++i){
			float f = ((rand()%10000)/10000.f)-0.5f;
			glf.push_back(f);
		}
		d.pos.data(glf);
		d.pos2.data(glf);
		d.acc.data(glf);
		d.state.data(glf);
		d.color.data(glf);
		d.screenposition.data(glf);
		d.oldscreenposition.data(glf);
		d.temp.data(glf);
		d.astate.data(glf);
		
		d.clline->genkernel("clline");
		d.clline->set("clline", 0, d.pos2);
		d.clline->genkernel("clline2");
		d.clline->set("clline2", 0, d.pos2);
		d.clline->genkernel("clzero");
		d.clline->genkernel("clboom");
		d.clline->genkernel("doagrow");
		d.clline->set("doagrow", 0, d.temp);
		d.clline->set("doagrow", 1, d.astate);
		
		d.clline->genkernel("cldgrow");
		d.clline->set("cldgrow", 0, d.pos2);
		d.clline->set("cldgrow", 2, d.astate);
		d.clline->set("cldgrow", 3, d.temp);
		d.clline->genkernel("clplane");
		d.clline->set("clplane", 0, d.pos2);
		d.clline->set("clplane", 2, d.astate);
		d.clline->set("clplane", 3, d.temp);
		d.clline->genkernel("clconway");
		d.clline->set("clconway", 0, d.pos2);
		d.clline->set("clconway", 2, d.astate);
		d.clline->set("clconway", 3, d.temp);
		d.clline->genkernel("clconwaydist");
		d.clline->set("clconwaydist", 0, d.pos2);
		d.clline->set("clconwaydist", 2, d.astate);
		d.clline->set("clconwaydist", 3, d.temp);
		d.clline->genkernel("doastate");
		d.clline->set("doastate", 0, d.temp);
		d.clline->set("doastate", 1, d.astate);

		d.clparticles->genkernel("particleengine");
		d.clparticles->set("particleengine", 0, d.pos);
		d.clparticles->set("particleengine", 1, d.acc);
		d.clparticles->set("particleengine", 2, d.state);
		d.clparticles->set("particleengine", 3, d.color);
		d.clparticles->set("particleengine", 4, 0.01f);
		d.clparticles->set("particleengine", 7, d.screenposition);
		d.clparticles->set("particleengine", 8, d.oldscreenposition);
		d.clparticles->set("particleengine", 9, 0.f);
		d.clparticles->set("particleengine",10, d.t);
		d.clparticles->set("particleengine",11, d.t1);
		d.clparticles->set("particleengine",12, d.t2);

		d.va.vertices(0,4, d.screenposition);
		d.va.vertices(1,4, d.oldscreenposition);
		d.va.vertices(2,4, d.color);	
		d.va2.vertices(0,4, d.pos2);
		d.va2.vertices(1,4, d.astate);
		
		d.reload = false;
		if(d.paused == false) d.m->pause(false);
	}
	virtual void init(shiva::state *instance, boost::any &data) const {
		instance->reg("kinput");
		instance->reg("resourcechange");
		data = boost::any(startdata());
		
		reload(data);
		
		auto &d = boost::any_cast<startdata&>(data);
		d.c.init(instance, "statedata/de/de.camera");
		

	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {
		
		auto &d = boost::any_cast<startdata&>(data);
		d.destroy();
	}
};
behaviour_add(particles);