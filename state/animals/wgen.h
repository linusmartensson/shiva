
#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"shaders.h"
#include"framebuffer.h"
#include"music.h"

struct hdrdata{
	FSShader fss;
	bool re;
	music *m;
	hdrdata() : m(new music("statedata/animals/animals.mp3")), re(false), fss("statedata/misc/testq.frag") {}
	void rebuild(){
		re = false;
		fss.rebuild("statedata/misc/testq.frag");
	}
};

class hdr : public shiva::behaviour {
		
public:
	void init(shiva::state *s, boost::any &data) const {
		data = hdrdata();
		auto d = boost::any_cast<hdrdata&>(data);
		s->reg("resourcechange");
		s->reg("kinput");
		d.m->pause(false);
	}
	void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<hdrdata&>(data);
		if(d.re) d.rebuild();
		d.fss.set("t", d.m->time());
		d.fss.display();
	}
	void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		auto &d = boost::any_cast<hdrdata&>(data);
		if(received.count("resourcechange")) d.re = true;
		if(received.count("kinput")){
			std::pair<int,bool> g = boost::any_cast<std::pair<int,bool>>(received["kinput"]);
			if(g.first == GLFW_KEY_ESC)
				shiva::core::stop();
		}
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(hdr); 
