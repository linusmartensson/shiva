#pragma once
#include"stdafx.h"
#include"state.h"
#include"clengine.h"
#include"shaders.h"
#include"cltree.h"
#include"data.h"

#include"controllers.h"
#include"framebuffer.h"

using namespace shiva::controls;

class cubic : public shiva::state {
	shiva::statelist sl;
	Program p, p2;
	ftree *ft;
	float starttime;
	float fadein;
	float distortfactor;
	hdrstate *hs;
public:
	cubic(hdrstate *hs) : hs(hs), ft(new ftree(StatelessCLProgram("xn=1;yt=1;",10), [](glm::mat4, VArray&, int){})),
					p("cubic.frag", "cube.geom", "cubic.vert"),
					p2("dreamscape_pt.frag", "dreamscape_pt.geom", "passthrough.vert"){
		starttime = ::t(0);
				fadein = 0;
		
		auto camera = new tcontrol<shiva::pdata<glm::mat4>>();
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::gtc::matrix_transform::lookAt(glm::vec3(30.0,80.0,-50), glm::vec3(0,0,0), glm::vec3(0,1,0))
				*glm::gtc::matrix_transform::scale(glm::mat4(1.0f), glm::vec3(10.0f))
				*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), shiva::core::physicsTime()*0.01f, glm::vec3(1.0f,0.f,0.f));
		}, ft->pmatrix(), t(1), t(7));
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(tslbi()*0.1f+15.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), shiva::core::physicsTime()*-0.14f, glm::vec3(0.0f,0.f,1.f))
				*glm::gtc::matrix_transform::lookAt(glm::vec3(0.f,0.f,-500+shiva::core::physicsTime()*0.1f), glm::vec3(0,0,0+shiva::core::physicsTime()*0.1f), glm::vec3(0,1,0))
				*glm::gtc::matrix_transform::scale(glm::mat4(1.0f), glm::vec3(10.0f));
		}, ft->pmatrix(), t(7), t(10));
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), shiva::core::physicsTime()*-0.14f, glm::vec3(0.0f,0.f,1.f))
				*glm::gtc::matrix_transform::lookAt(glm::vec3(20,20,-500+shiva::core::physicsTime()), glm::vec3(0,0,0+shiva::core::physicsTime()), glm::vec3(0,1,0))
				*glm::gtc::matrix_transform::scale(glm::mat4(1.0f), glm::vec3(10.0f));
		}, ft->pmatrix(), t(10), t(13));
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(tslbi()*0.1f+45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), shiva::core::physicsTime()*-0.14f, glm::vec3(0.0f,0.f,1.f))
				*glm::gtc::matrix_transform::lookAt(glm::vec3(tslbi()*0.1+10,tslbi()*0.1+10,-500+shiva::core::physicsTime()*0.1f), glm::vec3(0,0,0+shiva::core::physicsTime()*0.1f), glm::vec3(0,1,0))
				*glm::gtc::matrix_transform::scale(glm::mat4(1.0f), glm::vec3(10.0f));
		}, ft->pmatrix(), t(13), t(23));
		sl.add(camera);

		auto tmc = new tcontrol<shiva::data<float>>();
		sl.add(tmc);
		tmc->add<float>([&]{
			return (shiva::core::renderTime()-t(18))*0.001f+2.0f;
		}, hs->exposure(), t(18), t(23));

		auto mac = new tcontrol<shiva::data<float>>();
		mac->add<float>([&]{
			return (shiva::core::renderTime()-t(18))*0.7f/(t(23)-t(18))+0.3f;
		}, hs->mixOld(), t(18), t(23));

		
		p.bindAttrib(0, "pos");

		StatelessCLProgram terrain(loadFile("cubic.slcl"), 50000);
		auto ftc = new ftree(terrain, [&](glm::mat4 m, VArray &a, int i){
			
			p.use();
			p.set("distort", 0.f);
			p.set("mat", m);
			a.draw(0, i, 1, GL_POINTS);
		});
		ft->child(ftc);

		
		sl.add(ft);
	}
	~cubic(){
	}
	virtual void display(){
		hs->acc(1.0,0.95,1.0);
		hs->accblur(0,0.5);
		hs->mix(1.0,0.3,1.0);
		hs->toneMapping(1,2.0);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		p.set("screenSize", glm::vec2(shiva::core::width(), shiva::core::height()));
		if(fadein > 0.1f)
			glClearColor(fadein,fadein,fadein,0);
		else
			glClearColor(0,0,0,0);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0,0,0,0);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		sl.display();


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
	}
	
};