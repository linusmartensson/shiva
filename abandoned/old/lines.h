#pragma once
#include"stdafx.h"
#include"state.h"
#include"shaders.h"
#include"cltree.h"
#include"clengine.h"

class lines : public shiva::state {
	shiva::statelist sl;

	static const unsigned int particleCount = 7000;
	Program p, p2;
	Instance<FrameBuffer> fb;
	Instance<Texture> t;
	Instance<FSShader> fss;
	ftree *ft;
	float clearwhite;
public:
	lines(...) : p("flight_tri.frag","flight_tri.geom","passthrough.vert"),
		p2("flight_pt.frag","flight_pt.geom","passthrough.vert"),
		ft(new ftree(StatelessCLProgram("xn=1;yt=1;", 10), [](glm::mat4, VArray&, int){})) {
					

			auto timer = new shiva::controls::timercontrol();
			auto camera = new shiva::controls::control<shiva::data<glm::mat4>>;
			sl.add(ft);
			sl.add(timer);
			sl.add(camera);
			StatelessCLProgram	clprogram(loadFile("opening.slcl"), particleCount/5);
			for(int i=0;i<70*4;++i){
				auto ftc = new ftree(clprogram, [&](glm::mat4 m, VArray &a, int i){
					glEnable(GL_BLEND);
					glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
					p.use();
					p.bindAttrib(0, "pos");
					p.set("mat", m);

					glDisable(GL_DEPTH_TEST);
					glDepthMask(GL_FALSE);
					glDepthFunc(GL_LESS);
					a.draw(0,i, 1, GL_TRIANGLE_STRIP);
				
					p2.use();
					p2.bindAttrib(0, "pos");
					p2.set("mat", m);
					glBlendFunc(GL_ONE,GL_ONE);
					glDepthMask(GL_FALSE);
					a.draw(0,i, 1, GL_POINTS);
					glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
					glDisable(GL_BLEND);
					glDisable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
				});
				auto cam = new shiva::controls::control<shiva::data<glm::mat4>>;
				cam->add<glm::mat4>([&, i]() -> glm::mat4 {
					float s = std::max(-10.f,std::min(1.0f, (::t(23)-shiva::core::physicsTime())*0.005f+1.f));
					return glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), (360/5)*i + (360/10.f)*(i/5), glm::vec3(0,1,0))
					*glm::gtc::matrix_transform::translate(glm::mat4(1.0f), glm::vec3(30+(i/5)*30,0,0))
					* glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), shiva::core::physicsTime()*-0.05f, glm::vec3(0,1,0))
					* glm::gtc::matrix_transform::scale(glm::mat4(1.0f), glm::vec3(s,1,s));
				}, ftc->pmatrix());
				sl.add(cam);
				ft->child(ftc);
			}
			for(int i=2;i<23;++i)
				camera->addevent(timer->time(::t((float)i)));
			timer->listen(camera);
			camera->add<glm::mat4>([]{
				return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
					
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(0,150,-35), 
						glm::vec3(0,25,10), 
						glm::vec3(0,1,0)) * 
						glm::gtc::matrix_transform::rotate(glm::mat4(1.f), shiva::core::physicsTime()*0.01f, glm::vec3(0,1,0));
			}, ft->pmatrix());
			camera->add<glm::mat4>([]{
				return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
					
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(0,50,-35), 
						glm::vec3(0,70,10), 
						glm::vec3(0,1,0)) * 
						glm::gtc::matrix_transform::rotate(glm::mat4(1.f), shiva::core::physicsTime()*0.01f, glm::vec3(0,1,0));
			}, ft->pmatrix());
		fss = FSShader("hdr.frag");
		auto white = new shiva::controls::control<shiva::data<float>>;
		sl.add(white);
		
		fss->program().use();
		fss->set("maxbright", 20.0f+clearwhite*10.f);

		white->add<float>([&]{return std::max(0.0f, (shiva::core::physicsTime()-::t(23))*0.01f);}, clearwhite);
		stateUpdate();
		sl.display();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	}
	~lines(){
	}
	virtual void display(){
		
		p2.use();
		glUniform1f(p2.uniform("red"), 1.5f);
		glUniform1f(p2.uniform("green"), 1.8f);
		glUniform1f(p2.uniform("blue"), 1.3f);
		
		fb->render([&]{
			glDepthMask(GL_TRUE);
			//glClearColor(clearwhite*100.0f, clearwhite*100.0f, clearwhite*100.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glClearColor(0, 0, 0, 0.0f);
			sl.display();
		});
		
		fss->program().use();
		fss->set("maxbright", 20.0f+clearwhite*10.f);
		fss->set("exposure", 1.0f);
		fss->input("tex", t);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		fss->display();

	}

	virtual void update(){
		sl.update();
		ft->limit((shiva::core::physicsTime()-::t(15.f))*0.001f);

	}

	virtual bool stateUpdate(){
		bool ret = sl.stateUpdate();
		return ret;
	}

	virtual void reshape(int w, int h){
		sl.reshape(w,h);
		fb = FrameBuffer();
		t = *::auxtexture;
		fb->colorbuffer(0, t);
	}
	
};