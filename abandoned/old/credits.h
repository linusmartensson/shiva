#pragma once
#include"stdafx.h"
#include"state.h"
#include"cltree.h"
#include"shaders.h"
#include"misc.h"
#include"framebuffer.h"
#include"fonts.h"
#include<functional>

using namespace shiva;


class credits : public shiva::state {
	shiva::statelist sl;
	ftree *ft;
	Instance<FrameBuffer> fb;
	Instance<Texture> hdrtexture;
	Instance<FSShader> hdrrender;
	Instance<FSShader> fonttest;
	std::vector<Instance<Texture>> text;
//	Font f;
	Program p;
	glm::mat4 textcamera;
	float exposure;
	float starttime;
public:
	credits() : ft(new ftree(StatelessCLProgram("xn=1;yt=1", 1), [](glm::mat4 m, VArray &a, int i){return;})),
				 p("entryway_ribbon.frag", "passthrough_tri.geom", "passthrough.vert")
	//			 f("exmouth/exmouth_.ttf")
	{
		starttime=::t(81);
						//10 bars credits, l-system lines & birthing stars
		hdrrender = FSShader("hdr.frag");
		fonttest = FSShader("credits_font.frag");
		sl.add(ft);
		StatelessCLProgram beatline(loadFile("credits.slcl"), 500, loadFile("tools.slcl"));
		std::vector<ftree*> ch;
		/*
		f.pixelsize(200);
		//text.push_back(Instance<Texture>(f.render("6")));
		text.push_back(Instance<Texture>(f.render("Resten av scene")));
		text.push_back(Instance<Texture>(f.render("Folk i hoppborgen")));
		text.push_back(Instance<Texture>(f.render(" dlewen")));
		text.push_back(Instance<Texture>(f.render("Topatisen")));
		text.push_back(Instance<Texture>(f.render("CMR ")));
		text.push_back(Instance<Texture>(f.render("Sajko")));
		text.push_back(Instance<Texture>(f.render("Lusius")));
		text.push_back(Instance<Texture>(f.render("Music/Linus")));
		text.push_back(Instance<Texture>(f.render("Code/Linus")));
		text.push_back(Instance<Texture>(f.render("GFX/Linus")));*/

		auto camera = new shiva::controls::control<pdata<glm::mat4>>;
		sl.add(camera);
		camera->add<glm::mat4>([&]{return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
							*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), 180.f, glm::vec3(0.f,0.f,1.f))
							*glm::gtc::matrix_transform::lookAt(
										glm::vec3((shiva::core::physicsTime()-starttime)*0.010f-10,0,70), 
										glm::vec3((shiva::core::physicsTime()-starttime)*0.006f-10,0,0), 
										glm::vec3(0,1,0));}, ft->pmatrix());

		auto tcam = new shiva::controls::control<data<glm::mat4>>;
		sl.add(tcam);
		tcam->add<glm::mat4>([&]{
			return  glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
							*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), 0.f, glm::vec3(0.f,0.f,1.f))
							*glm::gtc::matrix_transform::lookAt(
								glm::vec3((shiva::core::physicsTime()-starttime)*-0.010f+300,0,70), 
								glm::vec3((shiva::core::physicsTime()-starttime)*-0.006f+300,-5,0), 
								glm::vec3(0,1,0))
							*glm::gtc::matrix_transform::scale(glm::mat4(1.0f), glm::vec3(10.0f));
		}, textcamera);

		auto xposr = new shiva::controls::control<data<float>>;
		xposr->add<float>([&]{
			return std::min(3.0f,(shiva::core::physicsTime()-starttime)*0.001f);
		}, exposure);
		sl.add(xposr);

		ch.push_back(ft);
		for(int i=0;i<100;++i){
			ftree *fu = new ftree(beatline, [&](glm::mat4 m, VArray &a, int i){
				p.use();
				p.bindAttrib(0, "pos");
				p.set("mat", m);
				a.draw(0, i, 1, GL_TRIANGLE_STRIP);
			});
			int pos = ch.size()>1?rand()%(ch.size()-1):0;
			fu->position(pos>0?(rand()%100)/100.f:0);
			ch[pos]->child(fu);
			ch.push_back(fu);
		}
	}
	~credits(){
	}
	virtual void display(){
		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		p.set("screenSize", glm::vec2(shiva::core::width(), shiva::core::height()));
		fb->render([&]{
			glClear(GL_COLOR_BUFFER_BIT);
			sl.display();
			int j=0;
			for(auto i=text.begin(); i!=text.end(); ++i, ++j){
				fonttest->transform(textcamera * glm::gtc::matrix_transform::translate(glm::mat4(1.0f), glm::vec3(j*3,0,0)));
				fonttest->input("text", *i);
				fonttest->set("visibility", 60.f+(text.size()-j)*40-(shiva::core::renderTime()-starttime)*0.01f);
				fonttest->clear(false);
				fonttest->display();
			}
		});
		hdrrender->input("tex", hdrtexture);
		hdrrender->set("maxbright", 1.f);
		hdrrender->set("exposure", exposure);

		glClear(GL_COLOR_BUFFER_BIT);
		hdrrender->display();

	}

	virtual void update(){
		ft->limit((shiva::core::physicsTime()-starttime)*0.0001f);
		sl.update();

	}

	virtual bool stateUpdate(){
		bool ret = sl.stateUpdate();

		return ret;
	}

	virtual void reshape(int w, int h){
		sl.reshape(w,h);
		fb = FrameBuffer();
		hdrtexture = *::hdrtexture;
		fb->colorbuffer(0, hdrtexture);
		
	}
	
};