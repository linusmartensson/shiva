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

class climactic : public shiva::state {
	shiva::statelist sl;
	Program p, p2;
	ftree *ft;
	float st;
	float fadein;
	float distortfactor;

	float fairy;
	hdrstate *hs;
public:
	climactic(hdrstate *hs, float sst) : st(sst), hs(hs), ft(new ftree(StatelessCLProgram("xn=1;yt=1;",10), [](glm::mat4, VArray&, int){})),
					p("climactic.frag", "catargas.geom", "passthrough.vert"),
					p2("pulsar.frag", "cube.geom", "passthrough.vert") {
		fadein = 0;
		p.bindAttrib(0, "pos");
		std::vector<ftree*> fts;
		StatelessCLProgram terrain(loadFile("climactic.slcl"), 10000);
		auto ftc = new ftree(terrain, [&](glm::mat4 m, VArray &a, int i){
			
			p.use();
			p.set("distort", 0.f);
			p.set("mat", m);
			a.draw(0, i, 1, GL_POINTS);
		});
		fts.push_back(ftc);
		ft->child(ftc);
		sl.add(ft);
		for(int j=0;j<500;++j){
			auto ftcc = new ftree(terrain, [&](glm::mat4 m, VArray &a, int i){
			
				p.use();
				p.set("distort", 0.f);
				p.set("mat", m);
				a.draw(0, i, 1, GL_POINTS);
				
				
			});
			int s = 0;
			if(fts.size()>10){
				s = fts.size()-10+rand()%(10);
			} else if(fts.size()){
				s = rand()%fts.size();
			}
			fts[s]->child(ftcc);
			ftcc->position((rand()%20+180)/200.f);
			ftcc->matrix(
				glm::rotate(glm::mat4(1.f), 90.f, glm::vec3(0,1,0))
				*glm::translate(glm::mat4(1.f), glm::vec3(100,100,0)))
				;
			fts.push_back(ftcc);
			
		}

		StatelessCLProgram pulsar(loadFile("pulsar.slcl"), 1000);
		ftc = new ftree(pulsar, [&](glm::mat4 m, VArray &a, int i){
			
				p2.use();
				p2.set("distort", 0.f);
				p2.set("mat", m);
				a.draw(0, i, 1, GL_POINTS);
		});
		ftc->position(0);
		
		auto beats = new tcontrol<shiva::pdata<glm::mat4>>();
		sl.add(beats);
		beats->add<glm::mat4>([&]{
			return glm::translate(glm::mat4(1.0f), glm::vec3(0,0,200*tslbi()+1000+(shiva::core::physicsTime()-st)*0.6))
					*glm::scale(glm::mat4(1.0f), glm::vec3(50,50,4000));
		}, ftc->pmatrix(), t(0)+st, t(1100)+st);

		ft->child(ftc);

		auto camera = new tcontrol<shiva::pdata<glm::mat4>>();
		sl.add(camera);
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(25.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 500000.0f)
				*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), (shiva::core::physicsTime()-st)*-0.05f, glm::vec3(0,0,1))
				*glm::gtc::matrix_transform::lookAt(
							glm::vec3(1200,-450,-1950+(shiva::core::physicsTime()-st)*3.7), 
							glm::vec3(700,-450,0+(shiva::core::physicsTime()-st)*3.7), 
							glm::vec3(0,1,0))
				;
		}, ft->pmatrix(), t(0)+st, t(500)+st);
		

	}
	virtual void display(){
		hs->acc(1.0,0.7,0);
		hs->accblur(1,0.5);
		hs->bloomblur(1,1);
		hs->mix(1.0,0.9,1);
		hs->toneMapping(10,1.1);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		p.set("screenSize", glm::vec2(shiva::core::width(), shiva::core::height()));
		if(fadein > 0.1f)
			glClearColor(fadein,fadein,fadein,0);
		else
			glClearColor(0,0,0,0);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0,0,0,0);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		sl.display();


	}

	virtual void update(){
		ft->limit((shiva::core::physicsTime()-st+tslbi()*10)*0.005f);
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