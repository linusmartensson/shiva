
#include<map>
#include<string>
#include<algorithm>
#include"state.h"
//#include"state/animals/lines.h"
//#include"state/animals/particles.h"
//#include"state/animals/house.h"
#include"state/animals/particles2.h"

//#include"state/animals/money.h"
#include"framebuffer.h"

struct hdrdata{
	FrameBuffer fb;
	Texture color, depth;
	FSShader fss;
	bool re;
	hdrdata() : re(false), color(8, GL_RGBA16F), depth(8, GL_DEPTH_COMPONENT32F), fss("statedata/animals/hdr.frag") {}
	void rebuild(){
		re = false;
		fss.rebuild("statedata/animals/hdr.frag");
	}
};

class hdr : public shiva::behaviour {
		
public:
	void init(shiva::state *s, boost::any &data) const {
		data = hdrdata();
		auto d = boost::any_cast<hdrdata&>(data);

		s->reg("resourcechange");
		d.fb.colorbuffer(0, d.color);
		d.fb.depthbuffer(d.depth);
	}
	void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<hdrdata&>(data);
		if(d.re) d.rebuild();
		d.fb.render([&]{
			std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();}); 
		});
		d.fss.program().use();
		d.fss.program().bindTexture("tex", d.color);
		d.fss.display();
	}
	void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		auto &d = boost::any_cast<hdrdata&>(data);

		if(received.count("resourcechange")) d.re = true;
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
		auto s = instance->create("hdr_node", "hdr");
		s->create("particle_node", "particles2");
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(root);