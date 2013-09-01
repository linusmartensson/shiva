#pragma once
#include"stdafx.h"
#include"state.h"
#include"clengine.h"
#include"shaders.h"
#include"cltree.h"
#include"data.h"
#include"controllers.h"
#include"framebuffer.h"

#include"fonts.h"

using namespace shiva::controls;

class outro : public shiva::state {
	shiva::statelist sl;
	Program p;
	ftree *ft;
	float st;
	float fadein;
	float distortfactor;
	Font f;
	Instance<FSShader> fonttest;
	std::vector<Instance<Texture>> text;
	
	glm::mat4 textcamera;
	float fairy;
	hdrstate *hs;
public:
	outro(hdrstate *hs, float sst) : st(sst), hs(hs), ft(new ftree(StatelessCLProgram("xn=1;yt=1;",10), [](glm::mat4, VArray&, int){})),
					p("statedata/jackhammer/outro/outro.frag", "statedata/jackhammer/outro/catargas.geom", "builtins/passthrough.vert"),
					f("statedata/jackhammer/outro/TECHNOID.TTF") {
				fadein = 0;
						
		fonttest = FSShader("statedata/jackhammer/outro/outrofont.frag");
		
		f.pixelsize(200);
		text.push_back(Instance<Texture>(f.render("")));
		text.push_back(Instance<Texture>(f.render("")));
		text.push_back(Instance<Texture>(f.render("Greetz to")));
		text.push_back(Instance<Texture>(f.render("Mental")));
		text.push_back(Instance<Texture>(f.render("Radd0x")));
		text.push_back(Instance<Texture>(f.render("LOLI")));
		text.push_back(Instance<Texture>(f.render("HC")));
		text.push_back(Instance<Texture>(f.render("Anona")));
		text.push_back(Instance<Texture>(f.render("Gussoh")));
		text.push_back(Instance<Texture>(f.render("Findus")));
		text.push_back(Instance<Texture>(f.render("31415")));
		text.push_back(Instance<Texture>(f.render("FoD")));
		text.push_back(Instance<Texture>(f.render("Nyan cat")));
		text.push_back(Instance<Texture>(f.render("HLR")));
		text.push_back(Instance<Texture>(f.render("fACERAPE")));
		text.push_back(Instance<Texture>(f.render("THMH")));
		text.push_back(Instance<Texture>(f.render("CMR")));
		text.push_back(Instance<Texture>(f.render("Kreativ")));
		text.push_back(Instance<Texture>(f.render("Everyone else")));
		text.push_back(Instance<Texture>(f.render("Music W0lfis")));
		text.push_back(Instance<Texture>(f.render("Code Linus")));
		

		p.bindAttrib(0, "pos");
		std::vector<ftree*> fts;
		StatelessCLProgram terrain(loadFile("statedata/jackhammer/outro/outro.slcl"), 12000);
		auto ftc = new ftree(terrain, [&](glm::mat4 m, VArray &a, int i){
			
			p.use();
			p.set("distort", 0.f);
			p.set("mat", m);
			a.draw(0, i, 1, GL_POINTS);
		});
		fts.push_back(ftc);
		ft->child(ftc);
		sl.add(ft);

		auto camera = new tcontrol<shiva::pdata<glm::mat4>>();
		sl.add(camera);
		camera->add<glm::mat4>([&]{
			return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 500000.0f)
				
				*glm::gtc::matrix_transform::lookAt(glm::vec3(300,1000,-1550+(shiva::core::physicsTime()-st)*0.8), glm::vec3(0,0,0+(shiva::core::physicsTime()-st)*0.8), glm::vec3(0,1,0))
				;
		}, ft->pmatrix(), t(0)+st, t(500)+st);
		
		auto tcam = new shiva::controls::tcontrol<shiva::data<glm::mat4>>;
		sl.add(tcam);
		tcam->add<glm::mat4>([&]{
			return  glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::gtc::matrix_transform::lookAt(glm::vec3(2,2,1.5-(shiva::core::physicsTime()-st)*0.002), glm::vec3(0,0,0-(shiva::core::physicsTime()-st)*0.002), glm::vec3(0,1,0))
							
				;
		}, textcamera, t(0), t(1000));
	}
	virtual void display(){
		hs->acc(1.0,0.7,0);
		hs->accblur(1,0.5);
		hs->bloomblur(1,1);
		hs->mix(1.0,0.4,1);
		hs->toneMapping(10,1);
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
		

		int j=0;
		for(auto i=text.begin(); i!=text.end(); ++i, ++j){
			fonttest->transform(textcamera * glm::gtc::matrix_transform::translate(glm::mat4(1.0f), glm::vec3(0,0,-j*4)));
			fonttest->input("text", *i);
			fonttest->set("visibility", 60.f);
			fonttest->clear(false);
			fonttest->display();
		}
		sl.display();
	}

	virtual void update(){
		ft->limit((shiva::core::physicsTime()-st)*0.0002f);
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