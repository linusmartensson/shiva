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

class dreamscape : public shiva::state {
	shiva::statelist sl;
	Program p,p2;
	ftree *ft;
	float starttime;
	float fadein;
	float distortfactor;
	float pulsarstr;
	hdrstate *hs;
public:
	dreamscape(hdrstate *hs, float sst) : starttime(sst), hs(hs), ft(new ftree(StatelessCLProgram("xn=1;yt=1;",10), [](glm::mat4, VArray&, int){})),
					p("dreamscape_pt.frag", "dreamscape_pt.geom", "passthrough.vert"),
					p2("pulsar2.frag", "cube.geom", "passthrough.vert") {
		
		
		auto camera = new tcontrol<shiva::pdata<glm::mat4>>();
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::gtc::matrix_transform::lookAt(
					glm::vec3(
						sin(shiva::core::physicsTime()*0.0001)*320,
						cos(shiva::core::physicsTime()*0.0001)*320,
						shiva::core::physicsTime()*0.3), glm::vec3(0,0,shiva::core::physicsTime()*0.3+2000), glm::vec3(0,1,0));
		}, ft->pmatrix(), t(0), t(1000));
		sl.add(camera);
				fadein = 0;
		
		p.bindAttrib(0, "pos");

		StatelessCLProgram terrain(loadFile("dreamscape.slcl"), 500000);
		ft->child(new ftree(terrain, [&](glm::mat4 m, VArray &a, int i){
			
			p.use();
			p.set("distort", 0.f);
			p.set("mat", m);
			a.draw(0, i, 1, GL_POINTS);
		}));
		
		sl.add(ft);

		
		StatelessCLProgram pulsar(loadFile("pulsar2.slcl"), 1000);
		auto ftc = new ftree(pulsar, [&](glm::mat4 m, VArray &a, int i){
			
				p2.use();
				p2.set("distort", 0.f);
				p2.set("mat", m);
				a.draw(0, i, 1, GL_POINTS);
		});
		ftc->position(0);
		
		auto beats = new tcontrol<shiva::pdata<glm::mat4>>();
		sl.add(beats);
		beats->add<glm::mat4>([&]{
			return glm::translate(glm::mat4(1.0f), glm::vec3(0,0,1000+(shiva::core::physicsTime())*0.3))
					*glm::scale(glm::mat4(1.0f), glm::vec3(5,5,4000));
		}, ftc->pmatrix(), t(0)+starttime, t(1100)+starttime);

		auto str = new tcontrol<shiva::data<float>>;
		sl.add(str);
		str->add<float>([&]{
			return pow(tslbi()/beatdist,4);
		}, pulsarstr, t(0)+starttime, t(1100)+starttime);

		ft->child(ftc);
	}
	~dreamscape(){
	}
	virtual void display(){
		p2.set("str", pulsarstr);
		hs->acc(1.0,0.95,1.0);
		hs->accblur(1.0,1.0);
		hs->mix(1.0,0.5,1.0);
		hs->toneMapping(100,0.2);

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
		ft->limit(1.0f);
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