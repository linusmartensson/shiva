	
#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"state/partevo/particles.h"
#include"framebuffer.h"
#include"texture.h"
#include"midi.h"

struct rootdata{
	midi mi;
	music *m;
	FSShader fss;
	Texture t, t2, t3, t4;
	bool reload;
	camera c;

	rootdata() : mi(midi("statedata/df/gating.mid")),
				 fss(Shader::version330()+Shader::noiselibs()+"statedata/df/hdr.frag"),
				 m(new music("statedata/df/DH-Final1.mp3")),
				 c(),
				 t(1, GL_R32F, GL_TEXTURE_2D, 128, 24),
				 t2(1, GL_R32F, GL_TEXTURE_2D, 24, 1), 
				 t3(1, GL_R32F, GL_TEXTURE_2D, 24, 1), 
				 t4(1, GL_R32F, GL_TEXTURE_2D, 1024, 1), reload(false) {
	}
	~rootdata(){
	}
	void refresh(){
		reload = false;
		fss.rebuild(Shader::version330()+Shader::noiselibs()+"statedata/df/hdr.frag");
		c.rebuild();
	}
};

class root : public shiva::behaviour {
	virtual void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		auto &d = boost::any_cast<rootdata&>(data);
		if(received.count("kinput")){
			std::pair<int,bool> g = boost::any_cast<std::pair<int,bool>>(received["kinput"]);
			if(g.first == GLFW_KEY_ESC)
				shiva::core::stop();
			if(g.first == GLFW_KEY_LEFT){
				d.m->time(d.m->time()-1.0);
			}
			if(g.first == GLFW_KEY_RIGHT){
				d.m->time(d.m->time()+1.0);
			}
			if(g.first == GLFW_KEY_SPACE && g.second){
				d.m->pause(!d.m->pause());
			}
		}
		
		if(received.count("resourcechange")){
			d.reload = true;
		}
		d.mi.run(d.m->time());
		d.c.run(received, d.m->time(), false);
		//if(d.m->time() > 155) shiva::core::stop();
	}
	virtual void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<rootdata&>(data);
		std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();});

		if(d.reload){
			d.refresh();
		}

		d.t.bind();
		for(int i=0;i<24;++i){
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, 128, 1, GL_RED, GL_FLOAT, d.mi.vn[i].data());
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		float f[24];
		float g[24];
		for(int i=0;i<24; ++i){
			f[i] = d.mi.v[i];
			g[i] = d.mi.pos[i];
		}
		d.t2.bind();
		glTexSubImage2D(GL_TEXTURE_2D, 0,0,0,24,1,GL_RED, GL_FLOAT, f);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		d.t3.bind();
		glTexSubImage2D(GL_TEXTURE_2D, 0,0,0,24,1,GL_RED, GL_FLOAT, g);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		auto h = d.m->spectrum();
		d.t4.bind();
		glTexSubImage2D(GL_TEXTURE_2D, 0,0,0,1024,1,GL_RED, GL_FLOAT, h.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		float speed = 3.0;
		glm::mat4 cam = glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.c.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.c.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f)));
		glm::mat4 rotcam = glm::transpose(glm::mat4_cast(glm::rotate(glm::quat(), d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), d.c.rot.x, glm::vec3(0,1,0))));
		glm::vec4 ptrans(d.c.trans, 0.0); 

		
		d.fss.program().bindTexture("bt", d.t,0);
		d.fss.program().bindTexture("track", d.t2,1);
		d.fss.program().bindTexture("pos", d.t3,2);
		d.fss.program().bindTexture("wform", d.t4,3);
		
		d.fss.set("rot", glm::mat3(rotcam));
		d.fss.set("trans", glm::vec3(ptrans*speed));
		d.fss.set("time", d.m->time());
		d.fss.program().set("camera", cam);

		d.fss.display();
	}

	virtual void init(shiva::state *instance, boost::any &data) const {
		//auto s = instance->create("hdr_node", "hdr");
		//s->create("particle_node", "particles");
		instance->reg("kinput");
		instance->reg("resourcechange");


		data = boost::any(rootdata());
		auto &d = boost::any_cast<rootdata&>(data);

		d.c.init(instance, "statedata/df/na.camera");

		d.m->pause(false);
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(root);