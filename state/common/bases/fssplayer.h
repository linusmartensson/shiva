#pragma once
#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"music.h"
#include"shaders.h"
#include"framebuffer.h"
#include"image.h"
#include"camera.h"
using namespace shiva;

class fssplayer : public behaviour {
public:
	struct fssdata{
		std::string f;
		FSShader fss;
		bool re;

		bool enable;
		float time;
		std::string timesrc;
		std::map<std::string, float> params;
		std::map<std::string, Texture*> tx;
		glm::mat4 transform;
		fssdata(std::string f, 
				std::string t, 
				std::map<std::string, float> iparams = std::map<std::string, float>(),
				std::map<std::string, Texture*> itx = std::map<std::string, Texture*>(),
				glm::mat4 transform = glm::mat4(1.0f)) : transform(transform), params(iparams), f(f), timesrc(t), fss(f), re(false), time(0.f), tx(itx) {
		}
		void rebuild(){
			re = false;
			fss.rebuild(f);

		}
	};
private:
	void init(shiva::state *s, boost::any &data) const {
		auto d = boost::any_cast<fssdata&>(data);
		s->reg("resourcechange");
		s->reg(d.timesrc);
		s->reg(s->get()+"/trans");
	}

	virtual void uninit(shiva::state *instance, boost::any &data) const {}

	void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<fssdata&>(data);
		if(d.re) d.rebuild();
		if(!d.params.count("on") || d.params["on"] < d.time){
			if(!d.params.count("off") || d.params["off"] > d.time){
				d.fss.clear(false);
				d.fss.set("time", d.time);
				std::for_each(d.params.begin(), d.params.end(), [&](std::pair<const std::string, float> p){
					d.fss.set(p.first,p.second);
				});
				std::for_each(d.tx.begin(), d.tx.end(), [&](std::pair<const std::string, Texture*> p){
					d.fss.input(p.first,*p.second);
				});
				d.fss.transform(d.transform);
				d.fss.display();
			}
		}
	}
	
	void run(shiva::eventmap &received, shiva::state *s, boost::any &data) const {
		auto &d = boost::any_cast<fssdata&>(data);
		if(received.count("resourcechange")) d.re = true;
		if(received.count(d.timesrc)){
			d.time = boost::any_cast<float>(received[d.timesrc]);
		}
		if(received.count(s->get()+"/trans")){
			d.transform = boost::any_cast<glm::mat4>(received[s->get()+"/trans"]);
		}
	}
};
behaviour_add(fssplayer);