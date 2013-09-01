#pragma once
#include"stdafx.h"
#include"state.h"
#include"shaders.h"
#include"cltree.h"
#include"clengine.h"

class flight : public shiva::state {
	shiva::statelist sl;

	static const unsigned int particleCount = 5000;
	Program p, p2, p3;
	Instance<FrameBuffer> fb;
	Instance<Texture> t, t2;
	Instance<FSShader> fss;
	ftree *ft;
	float red, hdr;
	float starttime;
public:
	flight() : p("flight_tri.frag","flight_tri.geom","passthrough.vert"),
		p2("flight_pt.frag","flight_pt.geom","passthrough.vert"),
		p3("flight_ext.frag","flight_pt.geom","passthrough.vert"),
		ft(new ftree(StatelessCLProgram("xn=1;yt=1;", 10), [](glm::mat4, VArray&, int){})) {
			starttime = ::t(69);
						//4 bars  tunnel chase
		//	timer->time(starttime+t(4)); 
						//4 bars  crazy lines chase
		//	timer->time(starttime+t(8)); 
						//4 bars  death
			auto timer = new shiva::controls::timercontrol();
			auto camera = new shiva::controls::control<shiva::data<glm::mat4>>;
			sl.add(ft);
			sl.add(timer);
			sl.add(camera);
			StatelessCLProgram	clprogram(loadFile("flight.slcl"), particleCount);
			ftree *f = new ftree(clprogram, [&](glm::mat4 m, VArray &a, int i){
					glEnable(GL_BLEND);
					glEnable(GL_DEPTH_TEST);
					glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
					glBlendFunc(GL_ONE,GL_ONE);
					glDepthMask(GL_TRUE);
					glDepthFunc(GL_LESS);
					p.use();
					p.bindAttrib(0, "pos");
					p.set("mat", m);
					
					glDisable(GL_DEPTH_TEST);
					glDepthMask(GL_FALSE);
					a.draw(0,i, 1, GL_TRIANGLE_STRIP);
				
					p2.use();
					p2.bindAttrib(0, "pos");
					p2.set("mat", m);
					a.draw(0,i, 1, GL_POINTS);
				});

			StatelessCLProgram linea("x=f*1000-500;z=0;y=0;", 1000);
			//StatelessCLProgram lineb("y=f*1000-500;z=0;x=0;", 1000);
			ft->child(f);
			for(int i=0;i<3000;++i){
				ftree *c = new ftree(linea, [&](glm::mat4 m, VArray &a, int j){
					glEnable(GL_BLEND);
					glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
					glBlendFunc(GL_ONE,GL_ONE);
					glDepthFunc(GL_LESS);
					glDisable(GL_DEPTH_TEST);
					glDepthMask(GL_FALSE);
					p3.use();
					p3.bindAttrib(0, "pos");
					p3.set("mat", m);
					a.draw(0,j, 1, GL_POINTS);
				});
				c->position((rand()%1000)/1000.f);
				f->child(c);
			}
			timer->listen(camera);
			
			for(int i=0;i<1000;++i){
				camera->addevent(timer->time(::t((float)i*2)));
			}
			camera->add<glm::mat4>([&]{
				return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::rotate(glm::mat4(1.0f), (float)(shiva::core::physicsTime()-starttime)*0.013f, glm::vec3(0,0,1))
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(0,0,-35+(shiva::core::physicsTime()-starttime)*0.03), 
						glm::vec3(0,0,10+(shiva::core::physicsTime()-starttime)*0.03), 
						glm::vec3(0,1,0));
			}, ft->pmatrix());
			camera->add<glm::mat4>([&]{
				return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::rotate(glm::mat4(1.0f), (float)(shiva::core::physicsTime()-starttime)*0.013f, glm::vec3(0,0,1))
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(10,0,-35+(shiva::core::physicsTime()-starttime)*0.03), 
						glm::vec3(0,0,10+(shiva::core::physicsTime()-starttime)*0.03), 
						glm::vec3(0,1,0));
			}, ft->pmatrix());
			camera->add<glm::mat4>([&]{
				return glm::gtc::matrix_transform::perspective(25.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::rotate(glm::mat4(1.0f), (float)(shiva::core::physicsTime()-starttime)*0.013f, glm::vec3(0,0,1))
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(0,10,-35+(shiva::core::physicsTime()-starttime)*0.03), 
						glm::vec3(0,0,10+(shiva::core::physicsTime()-starttime)*0.03), 
						glm::vec3(0,1,0));
			}, ft->pmatrix());
			camera->add<glm::mat4>([&]{
				return glm::gtc::matrix_transform::perspective(80.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::rotate(glm::mat4(1.0f), (float)(shiva::core::physicsTime()-starttime)*0.013f, glm::vec3(0,0,1))
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(0,10,-35+(shiva::core::physicsTime()-starttime)*0.04), 
						glm::vec3(0,0,-35+(shiva::core::physicsTime()-starttime)*0.04), 
						glm::vec3(1,0,0));
			}, ft->pmatrix());
			camera->add<glm::mat4>([&]{
				return glm::gtc::matrix_transform::perspective(120.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::rotate(glm::mat4(1.0f), (float)(shiva::core::physicsTime()-starttime)*0.013f, glm::vec3(0,0,1))
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(0,300,-35+(shiva::core::physicsTime()-starttime)*0.04), 
						glm::vec3(0,0,-15+(shiva::core::physicsTime()-starttime)*0.04), 
						glm::vec3(1,0,0));
			}, ft->pmatrix());
			auto redlevel = new shiva::controls::control<shiva::data<float>>;
			redlevel->add<float>([&]{
				return sin((shiva::core::physicsTime()-starttime)*0.001f)*0.5f+1;
			}, red);
			auto hdrcontrol = new shiva::controls::control<shiva::data<float>>;
			hdrcontrol->add<float>([&]{
				return sin((shiva::core::physicsTime()-starttime)*0.001f)*20+45;
			}, hdr);
			sl.add(hdrcontrol), sl.add(redlevel);
		fss = FSShader("hdr.frag");
			
	}
	~flight(){
	}
	virtual void display(){
		
		p2.use();
		glUniform1f(p2.uniform("red"), red*1.5f);
		glUniform1f(p2.uniform("green"), red*0.8f);
		glUniform1f(p2.uniform("blue"), red*0.3f);
		p3.use();
		glUniform1f(p3.uniform("red"), red*1.5f);
		glUniform1f(p3.uniform("green"), red*0.8f);
		glUniform1f(p3.uniform("blue"), red*0.3f);
		
		fb->render([&]{
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			sl.display();
		},t,t2);
		
		fss->program().use();
		fss->set("maxbright", hdr);
		fss->set("exposure", 1.0f);
		fss->input("tex", t);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		fss->display();

	}

	virtual void update(){
		sl.update();
		ft->limit((shiva::core::physicsTime()-starttime)*0.0015f);

	}

	virtual bool stateUpdate(){
		bool ret = sl.stateUpdate();

		return ret;
	}

	virtual void reshape(int w, int h){
		sl.reshape(w,h);
		fb = FrameBuffer();
		t = *::hdrtexture;
		t2 = *::depthtexture;

	}
	
};