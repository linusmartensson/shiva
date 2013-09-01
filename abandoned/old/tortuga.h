#pragma once
#include"stdafx.h"
#include"state.h"
#include"clengine.h"
#include"shaders.h"
#include"cltree.h"
#include"data.h"
#include"hdr.h"

#include"controllers.h"
#include"framebuffer.h"

using namespace shiva::controls;

class tortuga : public shiva::state {
	shiva::statelist sl;
	Program p;
	ftree *ft;
	float st;
	float fadein;
	float distortfactor;

	float fairy;
	float ang;
	hdrstate *hs;
public:
	tortuga(hdrstate *hs) : hs(hs), ft(new ftree(StatelessCLProgram("xn=1;yt=1;",10), [](glm::mat4, VArray&, int){})),
					p("statedata/jackhammer/tortuga/tortuga.frag", "common/shapes/cube.geom", "statedata/jackhammer/tortuga/tortuga.vert"){
		st=fadein=distortfactor=fairy=ang=0;
		st = ::t(0);
				fadein = 0;
		auto mac = new tcontrol<shiva::rdata<float>>();
		mac->add<float>([&]{
			return 1.0f-tslbir()*0.0003f;
		}, this->hs->accOld(), t(0)+st, t(23)+st);
		sl.add(mac);

		
		
		auto angc = new shiva::controls::tcontrol<shiva::pdata<float>>;
		sl.add(angc);

		ang = 45.f;
		angc->add<float>([&]{
			return 45.f;
		}, ang, t(0)+st, t(53-39)+st);
		angc->add<float>([&]{
			return 45-(shiva::core::physicsTime()-t(53-39)-st)*0.003f;
		}, ang, t(53-39)+st, t(57-39)+st);

		p.bindAttrib(0, "pos");

		StatelessCLProgram terrain(loadFile("statedata/jackhammer/tortuga/tortuga.slcl"), 50000);
		auto ftc = new ftree(terrain, [&](glm::mat4 m, VArray &a, int i){
			
			p.use();
			p.set("distort", 0.f);
			p.set("mat", m);
			a.draw(0, i, 1, GL_POINTS);
		});
		ft->child(ftc);
		sl.add(ft);
		ft->limit(1.0);
		ftc->limit(1.0);

		auto camera = new tcontrol<shiva::pdata<glm::mat4>>();
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(ang, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				
				*glm::gtc::matrix_transform::lookAt(glm::vec3(40.0,80.0,-250+(shiva::core::physicsTime()-st+tslbi()*0.2)*1.1f), glm::vec3(0,20,0+(shiva::core::physicsTime()-st+tslbi()*0.2)*1.1f), glm::vec3(0,1,0))
				*glm::gtc::matrix_transform::scale(glm::mat4(1.0f), glm::vec3(50.0f,1.f,300.f));
//				*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), 45-50/(1+shiva::core::physicsTime()*0.0003f), glm::vec3(0.0f,1.f,0.f));
		}, ftc->pmatrix(), t(0)+st, t(700)+st);
		sl.add(camera);
		
		StatelessCLProgram terra(loadFile("statedata/jackhammer/tortuga/tortuga.slcl"), 50000);
		ftc = new ftree(terra, [&](glm::mat4 m, VArray &a, int i){
			
			p.use();
			p.set("distort", 0.f);
			p.set("mat", m);
			a.draw(0, i, 1, GL_POINTS);
		});
		ft->child(ftc);
		camera = new tcontrol<shiva::pdata<glm::mat4>>();
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(ang, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				
				*glm::gtc::matrix_transform::lookAt(glm::vec3(00.0,00.0,-250+(shiva::core::physicsTime()-st)*1.1f), glm::vec3(-40,-60,0+(shiva::core::physicsTime()-st)*1.1f), glm::vec3(0,1,0))
				*glm::gtc::matrix_transform::scale(glm::mat4(1.0f), glm::vec3(10.0f,2.f,100.f));
//				*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), 45-50/(1+shiva::core::physicsTime()*0.0003f), glm::vec3(0.0f,1.f,0.f));
		}, ftc->pmatrix(), t(0)+st, t(700)+st);
		sl.add(camera);

		auto limiter = new tcontrol<shiva::pdata<int>>();
		limiter->add<int>([&]{
			return 0;
		}, ftc->plimit(), t(0)+st, t(3)+st);
		limiter->add<int>([&]{
			return 50000;
		}, ftc->plimit(), t(3)+st, t(23)+st);
		sl.add(limiter);

		auto exposure = new tcontrol<shiva::rdata<float>>();

		exposure->add<float>([&]{
			return 10.0f-(shiva::core::renderTime()-t(3)-st)*0.002f;
		}, hs->exposure(), t(3)+st, t(5)+st);
		exposure->add<float>([&]{
			return 10.0f;
		}, hs->exposure(), t(3)+st, t(5)+st);
		sl.add(exposure);

		fairy = 1.0f;
		auto fairytail = new tcontrol<shiva::data<float>>();
		fairytail->add<float>([&]{
			return 1.0f+(shiva::core::physicsTime()-st-t(65-39))*0.001f;
		}, fairy, t(65-39)+st, t(90)+st);
		fairytail->add<float>([&]{
			return 1.0f;
		}, fairy, t(0)+st, t(65-39)+st);
		sl.add(fairytail);
	}
	~tortuga(){
	}
	virtual void display(){
		hs->acc(1.0,0.95,1.0);
		hs->accblur(1,0.5);
		hs->mix(0.0,0.9,1.0);
		hs->toneMapping(1,5.0);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		p.set("screenSize", glm::vec2(shiva::core::width(), shiva::core::height()));
		p.set("fairy", fairy);
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