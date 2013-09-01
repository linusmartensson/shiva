
#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"shaders.h"
#include"state/dq/tk.h"
#include"framebuffer.h"

struct hdrdata{
	FrameBuffer fb;
	Texture color, depth;
	FSShader fss;
	bool reload;
	hdrdata() : color(8, GL_RGBA16F), depth(8, GL_DEPTH_COMPONENT32F), fss("statedata/dq/hdr.frag"), reload(false) {
		Log::info()<<"hello world"<<std::endl;
	} 
	void rebuild(){
		Log::info()<<"called rebuild"<<std::endl;
		fss.rebuild("statedata/dq/hdr.frag");
		reload = false;
	}
};

class hdr : public shiva::behaviour {
		
public:
	void init(shiva::state *s, boost::any &data) const {
		Log::info()<<"hello world2"<<std::endl;
		s->reg("resourcechange");
		data = hdrdata();
		auto d = boost::any_cast<hdrdata&>(data);
		d.fb.colorbuffer(0, d.color);
		d.fb.depthbuffer(d.depth);
	}
	void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<hdrdata&>(data);
		if(d.reload) d.rebuild();
		d.fb.render([&]{
			glClearColor(1.f,0.f,0.f,1.f);
			glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
			std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();}); 
		});
		glClearColor(1.f,0.f,0.f,1.f);
		glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
		d.fss.program().use();
		d.fss.set("resolution", glm::vec2(shiva::core::width(), shiva::core::height()));
		d.fss.program().bindTexture("tex", d.color);
		d.fss.display();
	}
	void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		auto &d = boost::any_cast<hdrdata&>(data);
		if(received.count("resourcechange")) d.reload = true;
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}

};
behaviour_add(hdr); 

class root : public shiva::behaviour {
	virtual void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {

	}
	virtual void render(shiva::childvec children, boost::any &data) const {
		std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();}); 
	}
	virtual void init(shiva::state *instance, boost::any &data) const {
		instance->create("demo_node", "tk");
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(root);