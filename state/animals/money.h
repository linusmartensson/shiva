#include"state.h"
#include"shaders.h"
#include"clengine.h"
#include"resourcetracker.h"
#include"framebuffer.h"
#include"music.h"
#include<algorithm>
#include<fstream>
#include"fonts.h"
#include"camera.h"
#include"image.h"
struct startdata{
	int pointcount;
	FrameBuffer fb;
	FSShader fss;
	Program p1,p2,p4;
	CLProgram *clparticles, *clline;
	PNGImage mimg;
	

	FSShader fss2;
	float last;
	std::string clp, cll;
	float aspectRatio;
	Texture t;
	Texture t1;
	Texture t2;
	Texture t3, t4;

	FrameBuffer tfb;
	
	Buffer<GLfloat> opos,pos,acc,state,astate,color,screenposition,oldscreenposition,pcam,pos2,temp;
	VArray va,va2;
	bool reload;
	int count;
	camera c;
	music *m;
	font *f;

	void rebuild(){
		  clparticles->rebuild(loadFile(clp, true));
		  clline->rebuild(loadFile(cll, true));
		  p1.rebuild("statedata/animals/money/minimal1.frag","statedata/animals/money/minimal1.geom","statedata/animals/money/minimal1.vert")
		, p2.rebuild("statedata/animals/money/minimal2.frag","statedata/animals/money/minimal2.geom","statedata/animals/money/minimal2.vert")
		, p4.rebuild("statedata/animals/minimal4.frag","statedata/animals/minimal4.geom","statedata/animals/minimal4.vert")
		;
		  c.rebuild();
		  fss2.rebuild("statedata/animals/money/test.frag");
		  mimg.rebuild();
		  fss.rebuild("statedata/animals/money/output.frag");
		  fb.colorbuffer(0, t);
		  fb.colorbuffer(1, t1);
		  fb.colorbuffer(2, t2);
		  fb.depthbuffer(t3);
		  t4.bind();
		  glTexImage2D(t4.texturedims, 0, GL_RGBA8, mimg.width(), mimg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, mimg.data());

		  m->time(0);
		  last = 0.f;
		  count = 0;
	}

	startdata() : reload(false), pointcount(60000), fss("statedata/animals/money/output.frag"),
	fss2("statedata/animals/money/test.frag")
	,mimg("statedata/animals/currency.png")
	,p1("statedata/animals/money/minimal1.frag","statedata/animals/money/minimal1.geom","statedata/animals/money/minimal1.vert")
	,p2("statedata/animals/money/minimal2.frag","statedata/animals/money/minimal2.geom","statedata/animals/money/minimal2.vert")
	,p4("statedata/animals/minimal4.frag","statedata/animals/minimal4.geom","statedata/animals/minimal4.vert")
	,count(0)
	,m(new music("statedata/animals/animals.mp3")), 
	t(1, GL_RGBA16F, GL_TEXTURE_2D), 
	t1(1, GL_RGBA16F, GL_TEXTURE_2D) , 
	t2(1, GL_RGBA16F, GL_TEXTURE_2D), f(0),
	t3(1, GL_DEPTH_COMPONENT32, GL_TEXTURE_2D),
	t4(1, GL_RGBA8, GL_TEXTURE_2D),
	last(0)
	{
		clp = "builtins/tools.cl statedata/animals/particleengine.cl";
		cll = "builtins/tools.cl statedata/animals/money/clline.cl";
		clparticles = new CLProgram(loadFile(clp));
		clline = new CLProgram(loadFile(cll));
		fb.colorbuffer(0, t);
		fb.colorbuffer(1, t1);
		fb.colorbuffer(2, t2);
		fb.depthbuffer(t3);
		t4.bind();
		glTexImage2D(t4.texturedims, 0, GL_RGBA8, mimg.width(), mimg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, mimg.data());
	}
	void destroy(){
		delete clparticles;
		delete clline;
		delete m;
		delete f;
	}
};
class money : public shiva::behaviour {
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
			if(g.first == GLFW_KEY_RIGHT && g.second == true){
				d.m->time(d.m->time()+1.0f);
				move = true;
			}
			if(g.first == GLFW_KEY_LEFT && g.second == true){
				d.m->time(d.m->time()-1.0f);
				move = true;
			}
			if(g.first == GLFW_KEY_SPACE && g.second == true){
				d.m->pause(!d.m->pause());
			}
		}
		
		d.aspectRatio = 16.f/9.f;
		
		d.c.run(received, d.m->time(), d.m->pause());
	}
	virtual void render(shiva::childvec children, boost::any &data) const {
		startdata &d = (boost::any_cast<startdata&>(data));
		if(d.reload) reload(data);

		glDisable(GL_BLEND);
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE,GL_ZERO);

		float speed = 0.1;
		glm::mat4 cam = glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.c.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.c.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f)));
		glm::mat4 rotcam = glm::transpose(glm::mat4_cast(glm::rotate(glm::quat(), d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), d.c.rot.x, glm::vec3(0,1,0))));
		glm::vec4 ptrans(d.c.trans, 0.0); 
		
		float toff = d.m->time();
		
		d.clline->set("clconway", 1, toff);
		d.clline->run("clconway", d.pointcount);	
		
		d.fss2.set("rot", glm::mat3(rotcam));
		d.fss2.set("trans", glm::vec3(ptrans*speed));
		d.fss2.set("time", d.m->time());
		d.fss2.program().set("camera", cam);
	
		d.fb.render([&]{
			glDisable(GL_BLEND);
			glClearColor(0,0,0,1);
			glClear(GL_DEPTH_BUFFER_BIT);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			d.p4.bindAttrib(0, "position");
			d.p4.bindAttrib(1, "astate");
			d.p4.use();
			d.p4.set("camera", cam);
			d.p4.set("time", d.m->time());
			d.va2.draw(0,d.pointcount,1,GL_POINTS);	
			d.fss2.clear(false);
			d.fss2.display();
			
		});
		
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.f,0.f,0.f,0.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		d.pcam.data(16, glm::value_ptr(cam));
		
		d.clparticles->set("particleengine", 9, d.m->pause()?1.f:0.f);
		d.clparticles->set("particleengine", 5, toff);
		d.clparticles->set("particleengine", 6, d.pcam);
		d.clparticles->run("particleengine", d.pointcount);
		
		//Point rendering.
		d.p2.bindAttrib(0, "position");
		d.p2.bindAttrib(1, "position2");
		d.p2.bindAttrib(2, "color");
		d.p2.bindAttrib(3, "acceleration");
		d.p2.bindTexture("t", d.t4, 0);

		d.p1.bindAttrib(0, "position");
		d.p1.bindAttrib(1, "position2");
		d.p1.bindAttrib(2, "color");
		d.p2.bindAttrib(3, "acceleration");
		
		d.p1.use();
		d.p1.set("time", d.m->time());
		d.va.draw(d.pointcount/2,d.pointcount/2-1,1,GL_POINTS);
		d.p2.use();
		d.p2.set("time", d.m->time());
		d.va.draw(0,d.pointcount/2,1,GL_POINTS);

		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		d.fss.set("time", toff);
		d.fss.set("vis", clamp(2.f-toff/10.f,0.f,0.8f));
		d.fss.set("range", 50000.f);
		glBlendFunc(GL_ONE,GL_ONE);
		
		d.fss.set("resolution", glm::vec2((float)shiva::core::width(), (float)shiva::core::height()));
		d.fss.program().bindTexture("w", d.t, 0);
		d.fss.program().bindTexture("a", d.t1, 1);
		d.fss.program().bindTexture("c", d.t2, 2);
		d.fss.clear(false);
		d.fss.display();
		
	}
	void reload(boost::any &data) const {
		auto &d = boost::any_cast<startdata&>(data);
		
		d.rebuild();
		
		d.aspectRatio = 16.f/9.f;
		
		float de = (float)d.m->time()*0.4f;

		std::vector<GLfloat> glf;
		for(int i=0;i<4*d.pointcount;++i){
			float f = ((rand()%10000)/10000.f)-0.5f;
			glf.push_back(f);
		}
		d.pos.data(glf);
		d.opos.data(glf);
		d.pos2.data(glf);
		d.acc.data(glf);
		d.state.data(glf);
		d.color.data(glf);
		d.screenposition.data(glf);
		d.oldscreenposition.data(glf);
		d.temp.data(glf);
		d.astate.data(glf);
		
		d.clline->genkernel("clzero");
		d.clline->genkernel("clboom");
		d.clline->genkernel("clconway");
		d.clline->set("clconway", 0, d.pos2);
		d.clline->set("clconway", 2, d.astate);
		d.clline->set("clconway", 3, d.temp);

		d.clparticles->genkernel("particleengine");
		d.clparticles->set("particleengine", 0, d.pos);
		d.clparticles->set("particleengine", 1, d.acc);
		d.clparticles->set("particleengine", 2, d.state);
		d.clparticles->set("particleengine", 3, d.color);
		d.clparticles->set("particleengine", 4, 0.01f);
		d.clparticles->set("particleengine", 7, d.screenposition);
		d.clparticles->set("particleengine", 8, d.oldscreenposition);
		d.clparticles->set("particleengine", 9, 0.f);
		d.clparticles->set("particleengine",10, d.opos);
		d.clparticles->set("particleengine",11, d.t);
		d.clparticles->set("particleengine",12, d.t1);
		d.clparticles->set("particleengine",13, d.t2);
		
		d.va.vertices(0,4, d.screenposition);
		d.va.vertices(1,4, d.oldscreenposition);
		d.va.vertices(2,4, d.color);	
		d.va.vertices(3,4, d.acc);	
		
		d.va2.vertices(0,4, d.pos2);
		d.va2.vertices(1,4, d.astate);
		
		
		d.clline->set("clzero", 1, 0.f);
		d.clline->set("clzero", 2, 0.f);
		d.clline->set("clzero", 3, 0.f);
		d.clline->set("clzero", 4, 0.f);

		d.clline->set("clboom", 0, d.astate);
		d.clline->set("clboom", 1, 342.f);
		d.clline->set("clboom", 2, (float)rand());
		d.clline->set("clboom", 3, 0.5f);

		d.clline->run("clboom", d.pointcount);

		d.reload = false;
		d.m->pause(false);
	}
	virtual void init(shiva::state *instance, boost::any &data) const {
		instance->reg("kinput");
		instance->reg("resourcechange");
		data = boost::any(startdata());
		
		reload(data);
		
		auto &d = boost::any_cast<startdata&>(data);
		d.c.init(instance, "statedata/animals/0.camera");
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {
		
		auto &d = boost::any_cast<startdata&>(data);
		d.destroy();
	}
};
behaviour_add(money);