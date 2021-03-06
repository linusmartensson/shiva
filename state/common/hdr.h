
#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"state/partevo/particles.h"
#include"framebuffer.h"

struct hdrdata{
	FrameBuffer fb;
	Texture color, depth;
	FSShader fss;
	hdrdata() : color(8, GL_RGBA16F), depth(8, GL_DEPTH_COMPONENT32F), fss("statedata/de/hdr.frag") {} 
};

class hdr : public shiva::behaviour {
		
public:
	void init(shiva::state*, boost::any &data) const {
		data = hdrdata();
		auto d = boost::any_cast<hdrdata&>(data);
		d.fb.colorbuffer(0, d.color);
		d.fb.depthbuffer(d.depth);
	}
	void render(shiva::childvec children, boost::any &data) const {
		auto d = boost::any_cast<hdrdata&>(data);
		d.fb.render([&]{
			std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();}); 
		});
		d.fss.program().use();
		d.fss.program().bindTexture("tex", d.color);
		d.fss.display();
	}
	void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}

};
behaviour_add(hdr); 