#pragma once
#include"stdafx.h"
#include"state.h"
#include"shaders.h"
#include"cltree.h"
#include"clengine.h"

class entrytunnel : public shiva::state {
	shiva::statelist sl;

	static const unsigned int particleCount = 100000;
	Program p, p2;
	Instance<FrameBuffer> fb;
	Instance<Texture> t, t2;
	Instance<FSShader> fss;
	ftree *ft;
	float hdr;
	float starttime;
public:
	entrytunnel(...) : p("entrytunnel_tri.frag","flight_tri.geom","passthrough.vert"),
		p2("flight_pt.frag","flight_pt.geom","passthrough.vert"),
		ft(new ftree(StatelessCLProgram("xn=1;yt=1;", 10), [](glm::mat4, VArray&, int){})) {
			starttime = ::t(38);
			auto timer = new shiva::controls::timercontrol();
			auto camera = new shiva::controls::control<shiva::data<glm::mat4>>;
			sl.add(ft);
			sl.add(timer);
			sl.add(camera);
			StatelessCLProgram	clprogram(loadFile("entrytunnel.slcl"), particleCount);
			ft->child(new ftree(clprogram, [&](glm::mat4 m, VArray &a, int i){
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE,GL_ONE);
				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				glDepthFunc(GL_LESS);
				glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
				p.use();
				p.bindAttrib(0, "pos");
				p.set("mat", m);

				a.draw(0,i, 1, GL_TRIANGLE_STRIP);
				
				p2.use();
				p2.bindAttrib(0, "pos");
				p2.set("mat", m);
				a.draw(0,i, 1, GL_POINTS);
			}));
			
			camera->add<glm::mat4>([&]{
				return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				//*glm::rotate(glm::mat4(1.0f), (float)(shiva::core::physicsTime()-starttime)*0.013f, glm::vec3(0,0,1))
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(0,0,-120+(shiva::core::physicsTime()-starttime)*0.04), 
						glm::vec3(0,0,10+(shiva::core::physicsTime()-starttime)*0.04), 
						glm::vec3(0,1,0));
			}, ft->pmatrix());
			auto hdrcontrol = new shiva::controls::control<shiva::data<float>>;
			hdrcontrol->add<float>([&]{
				return std::max(0.0f,((shiva::core::physicsTime()-starttime-::t(6))*0.001f)*10);
			}, hdr);
			sl.add(hdrcontrol);
		fss = FSShader("hdr.frag");
		stateUpdate();
			sl.display();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
	}
	~entrytunnel(){
	}
	virtual void display(){
		
		p2.use();
		glUniform1f(p2.uniform("red"), 1.5f);
		glUniform1f(p2.uniform("green"), 0.8f);
		glUniform1f(p2.uniform("blue"), 1.8f);
		
		fb->render([&]{
			glDepthMask(GL_TRUE);
			if(shiva::core::physicsTime() > starttime+::t(6)){
				glClearColor(hdr*10.0f,hdr*10.0f,hdr*10.0f,1.0f);
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0,0,0,0);
			sl.display();
		},t,t2);
		
		fss->program().use();
		fss->set("maxbright", std::max(1.0f,20.0f-hdr));
		fss->set("exposure", 1.0f);
		fss->input("tex", t);
		fss->clear();
		
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ONE);
		glDepthMask(GL_FALSE);
		
		fss->display();

	}

	virtual void update(){
		sl.update();
		ft->limit((shiva::core::physicsTime()-starttime)*0.000003f);

	}

	virtual bool stateUpdate(){
		bool ret = sl.stateUpdate();

		return ret;
	}

	virtual void reshape(int w, int h){
		sl.reshape(w,h);
		fb = FrameBuffer();
		t = *::auxtexture;
		t2 = *::depthtexture;

	}
	
};