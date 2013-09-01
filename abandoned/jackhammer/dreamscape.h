#pragma once
#include"stdafx.h"
#include"state.h"
#include"cltree.h"

#include"controllers.h"
#include"framebuffer.h"

using namespace shiva::controls;

class dreamscape : public shiva::state {
	shiva::statelist sl;
	Program p,p2;
	ftree *ft;
	Instance<Texture> hdrtexture;
	Instance<FSShader> hdrrender;
	Instance<FrameBuffer> fb;
	float starttime;
	float fadein;
	float distortfactor;
public:
	dreamscape() : ft(new ftree(StatelessCLProgram("xn=1;yt=1;",10), [](glm::mat4, VArray&, int){})),
					p("dreamscape_pt.frag", "dreamscape_pt.geom", "passthrough.vert"),
					p2("dreamscapesky_pt.frag", "dreamscapesky_pt.geom", "passthrough.vert"){
		starttime = ::t(45);
		auto timer = new shiva::controls::timercontrol;
						//12 bars explore
		timer->time(t(12)+starttime); 
						//8 bars  crazy lines start, world distorts
		timer->time(t(12+8)+starttime); 
						//4 bars  world crash
		
		hdrrender = FSShader("dreamscapehdr.frag");
		auto beattimer = new shiva::controls::timercontrol;
		
		auto camera = new control<pdata<glm::mat4>>();
		beattimer->listen(camera);
		sl.add(beattimer);
		for(int i=0;i<8;++i){
			camera->addevent(beattimer->time(::t(i*2.f)+starttime));
		}
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::gtc::matrix_transform::lookAt(glm::vec3(1700*cos((shiva::core::physicsTime()-starttime)*0.0004f),1200,1700*sin((shiva::core::physicsTime()-starttime)*0.0004f)), glm::vec3(0,350,0), glm::vec3(0,1,0));
		}, ft->pmatrix());
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(1700*cos((shiva::core::physicsTime()-starttime)*0.0004f),1200,1700*sin((shiva::core::physicsTime()-starttime)*0.0004f)), 
						glm::vec3(1700*-sin((shiva::core::physicsTime()-starttime)*0.0004f),350,1700*cos((shiva::core::physicsTime()-starttime)*0.0004f)), 
						glm::vec3(0,1,0));
		}, ft->pmatrix());
		sl.add(camera);

		auto distcontrol = new shiva::controls::control<data<float>>;
		sl.add(distcontrol);
		distcontrol->add<float>([&]{return std::max(0.0f, (shiva::core::physicsTime()-starttime-::t(12))*0.001f);}, distortfactor);

		auto fadeincontrol = new shiva::controls::control<data<float>>;
		sl.add(fadeincontrol);
		fadeincontrol->add<float>([&]{return clamp((shiva::core::physicsTime()-starttime)*-1.f+500.f,0.f,500.f);}, fadein);


		StatelessCLProgram terrain(loadFile("dreamscape.slcl"), 3000000);
		ft->child(new ftree(terrain, [&](glm::mat4 m, VArray &a, int i){
			
			p.use();
			p.set("distort", distortfactor);
			p.bindAttrib(0, "pos");
			p.set("mat", m);
			a.draw(0, i, 1, GL_POINTS);
		}));
		StatelessCLProgram sky(loadFile("dreamscapesky.slcl"), 500000);
		ft->child(new ftree(sky, [&](glm::mat4 m, VArray &a, int i){
			
			p2.use();
			p2.set("distort", distortfactor);
			p2.bindAttrib(0, "pos");
			p2.set("mat", m);
			a.draw(0, i, 1, GL_POINTS);
		}));
		sl.add(ft);
		stateUpdate();
		sl.display();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	~dreamscape(){
	}
	virtual void display(){
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		p.set("screenSize", glm::vec2(shiva::core::width(), shiva::core::height()));
		fb->render([&]{
			if(fadein > 0.1f)
				glClearColor(fadein,fadein,fadein,0);
			else
				glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0,0,0,0);
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			sl.display();
		});

		hdrrender->input("tex", hdrtexture);
		hdrrender->set("maxbright", 20.f);
		hdrrender->set("res", glm::vec2(shiva::core::width(), shiva::core::height()));
		hdrrender->set("distort", distortfactor);
		hdrrender->set("exposure", 0.5f);
		
		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glClear(GL_COLOR_BUFFER_BIT);
		hdrrender->display();

	}

	virtual void update(){
		ft->limit((shiva::core::physicsTime()-starttime)*0.01f);
		sl.update();

	}

	virtual bool stateUpdate(){
		bool ret = sl.stateUpdate();

		return ret;
	}

	virtual void reshape(int w, int h){

		sl.reshape(w,h);
		hdrtexture = *::hdrtexture;
		fb = FrameBuffer();
		fb->colorbuffer(0, hdrtexture);
	}
	
};