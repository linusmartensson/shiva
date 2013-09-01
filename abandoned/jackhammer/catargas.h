#pragma once
#include"stdafx.h"
#include"state.h"
#include"clengine.h"
#include"shaders.h"
#include"cltree.h"
#include"data.h"
#include"hdr.h"
#include"camera.h"

#include"controllers.h"
#include"controllables.h"
#include"framebuffer.h"
#include"mixer.h"

using namespace shiva::controls;

class catargas : public shiva::state {
	Program p;
	ftree *ft;
	float fadein;
	float distortfactor;

	float fairy;
	float time;
	hdrstate *hs;
	StatelessCLProgram terrain;
	
	float xd, yd;
	glm::vec3 trans;

public:
	catargas(hdrstate *hs) : hs(hs), ft(new ftree(StatelessCLProgram("xn=1;yt=1;",10), [](glm::mat4, VArray&, int){})),
					p("catargas.frag", "catargas.geom", "passthrough.vert"),
					yd(0), xd(0), terrain(loadFile("catargas.slcl"), 1200) {
		fadein = 0;
		
		fairy = 0;
		auto c = new camera("catargas", trans, xd, yd, 'W', 'S', 'A', 'D');
		sl.add(c);
		
		auto m = new mixer;
		sl.add(m);
		m->add<float>(4, "fairy", fairy, [](float f, int i){return f+i;});
		//shiva::controllable::make_control<float>(sl, "catargas_test.ip", fairy, [](float f, int i){return f+i;}, shiva::controllable::wheel, 0, 0, false, GLFW_KEY_ENTER, GLFW_KEY_F5, GLFW_KEY_F11, GLFW_KEY_F12);

				p.bindAttrib(0, "pos");
		std::vector<ftree*> fts;

		auto ftc = new ftree(terrain, [&](glm::mat4 m, VArray &a, int i){
			
			p.use();
			p.set("distort", 0.f);
			p.set("mat", m);
			a.draw(0, i, 1, GL_POINTS);
		});
		fts.push_back(ftc);
		ft->child(ftc);
		sl.add(ft);
		for(int j=0;j<2000;++j){
			auto ftcc = new ftree(terrain, [&](glm::mat4 m, VArray &a, int i){
			
				p.use();
				p.set("distort", 0.f);
				p.set("mat", m);
				a.draw(0, i, 1, GL_POINTS);
				
				
			});
			fts[rand()%fts.size()]->child(ftcc);
			ftcc->position((rand()%50+150)/200.f);
			fts.push_back(ftcc);
			
		}

		auto camera = new tcontrol<shiva::pdata<glm::mat4>>();
		sl.add(camera);
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), yd, glm::vec3(1,0,0))*glm::rotate(glm::quat(), xd, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), trans)
				;
		}, ft->pmatrix(), t(1), t(500));
		

	}
	virtual void kinput(int k, bool s){

		if(k == GLFW_KEY_HOME && !s){
			terrain.rebuild(loadFile("catargas.slcl"));
			p.rebuild("catargas.frag", "catargas.geom", "passthrough.vert");
		}

		sl.kinput(k,s);
	}
	virtual void display(){
		hs->acc(0,0.95f,0.3);
		hs->accblur(1,1);
		hs->bloomblur(1,1);
		hs->mix(1,fairy*0.1f,1);
		hs->toneMapping(1,1);
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
		ft->limit((shiva::core::physicsTime())*0.0002f);
		
		sl.update();
	}
};