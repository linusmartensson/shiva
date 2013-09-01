#pragma once
#include"stdafx.h"
#include"state.h"
#include"cltree.h"
#include"shaders.h"
#include"framebuffer.h"
#include"entrytunnel.h"
#include<functional>

using namespace shiva;


class entryway : public shiva::state {
	shiva::statelist sl;
	ftree *ft;
	Instance<FrameBuffer> fb;
	Instance<Texture> hdrtexture;
	Instance<FSShader> hdrrender;

	Program p;
public:
	entryway() : ft(new ftree(StatelessCLProgram("xn=1;yt=1", 1), [](glm::mat4 m, VArray &a, int i){return;})),
				 p("entryway_ribbon.frag", "passthrough_tri.geom", "passthrough.vert")
	{
		auto timer = new shiva::controls::timercontrol;
		sl.add(timer);
							//9 bars beatlines
		timer->time(t(38)); 
							//3 bars tunnel
		timer->time(t(41)); 
						//4 bars  exit tunnel, early view
		hdrrender = FSShader("hdr.frag");
		sl.add(ft);
		StatelessCLProgram beatline("float odd = (float)((i)%2); \
									float p = f*2*3.14159; \
									\
									float n = (pow(sin(p),2.f)>0.95f?1.f:0.f); \
									y=sig((f-0.5f)*30.f)*15.f+odd*f*4; \
									x=f*120; \
									xn=x; \
									yn=y; \
									yt=1", 10000, "float sig(float t){return 1.f/(1.f+pow(2.71828f,-t)); }");
		std::vector<ftree*> ch;

		auto camera = new shiva::controls::control<pdata<glm::mat4>>;
		sl.add(camera);
		camera->addevent(0);
		timer->listen(camera);
		camera->add<glm::mat4>([&]{return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
							*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), (shiva::core::physicsTime()-::t(29))*0.01f, glm::vec3(0.f,0.f,1.f))
							*glm::gtc::matrix_transform::lookAt(
										glm::vec3((shiva::core::physicsTime()-::t(29))*0.01f-10,0,70), 
										glm::vec3((shiva::core::physicsTime()-::t(29))*0.01f-10,0,0), 
										glm::vec3(0,1,0));}, ft->pmatrix());
		camera->add<glm::mat4>([&]{return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
							*glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), (shiva::core::physicsTime()-::t(29))*0.01f, glm::vec3(0.f,0.f,1.f))
							*glm::gtc::matrix_transform::lookAt(
										glm::vec3((shiva::core::physicsTime()-::t(29))*0.01f-10,0,70-(shiva::core::physicsTime()-::t(38))*0.05), 
										glm::vec3((shiva::core::physicsTime()-::t(29))*0.01f-10,0,0-(shiva::core::physicsTime()-::t(38))*0.05), 
										glm::vec3(0,1,0));}, ft->pmatrix());


		ch.push_back(ft);
		for(int i=0;i<40;++i){
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
		auto ss = new shiva::controls::control<state>;
		sl.add(ss);

		ss->add(new statelist);
		ss->addevent(0);
		ss->add(new entrytunnel);

		timer->listen(ss);
	}
	~entryway(){
	}
	virtual void display(){
		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		p.set("screenSize", glm::vec2(shiva::core::width(), shiva::core::height()));
		fb->colorbuffer(0, hdrtexture);
		fb->render([&]{
			glClear(GL_COLOR_BUFFER_BIT);
			sl.display();
		});

		hdrrender->input("tex", hdrtexture);
		hdrrender->set("maxbright", 1.f);
		hdrrender->set("exposure", 5.f);

		glClear(GL_COLOR_BUFFER_BIT);
		hdrrender->display();

	}

	virtual void update(){
		ft->limit((shiva::core::physicsTime()-::t(29))*0.0001f);
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

	}
	
};