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

class drawpath : public behaviour {
public:
	struct pathdata{
		bool re;

		bool enable;
		float time;
		GLuint drawtype;
		std::string timesrc, camerasrc;
		std::map<std::string, float> params;

		VArray va;
		std::map<std::string, Buffer<float>*> data;
		std::map<std::string, Texture*> tx;
		glm::mat4 camera;

		Program pathrenderer;
		std::string frag,geom,vert;
		pathdata(
				std::string frag, std::string geom, std::string vert,
				std::string t, std::string c,
				std::map<std::string, float> iparams = std::map<std::string, float>(),
				std::map<std::string, Buffer<float>*> data = std::map<std::string, Buffer<float>*>(),
				std::map<std::string, Texture*> tx = std::map<std::string, Texture*>(), GLuint drawtype = GL_TRIANGLE_STRIP) : drawtype(drawtype), params(iparams), data(data), timesrc(t), camerasrc(c), re(false), time(0.f), pathrenderer(frag,geom,vert), frag(frag), geom(geom),vert(vert), tx(tx) {
		}
		void rebuild(){
			re = false;
			pathrenderer.rebuild(frag,geom,vert);
		}
	};
private:
	void init(shiva::state *s, boost::any &data) const {
		auto d = boost::any_cast<pathdata&>(data);
		s->reg("resourcechange");
		s->reg(d.timesrc);
		s->reg(d.camerasrc);
	}

	virtual void uninit(shiva::state *instance, boost::any &data) const {}

	void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<pathdata&>(data);
		if(d.re) d.rebuild();
		if(!d.params.count("on") || d.params["on"] < d.time){
			if(!d.params.count("off") || d.params["off"] > d.time){
				uint minsz = std::numeric_limits<int>::max();
				int i=0;
				std::for_each(d.data.begin(), d.data.end(), [&](std::pair<std::string, Buffer<float>*> p){
					d.va.vertices(d.pathrenderer.getAttribLocation(p.first), p.second->elemsperitem, *p.second, false, 0, 0);
					minsz = std::min(minsz, p.second->size()/p.second->elemsperitem);
					++i;
				});
				if(i>0){
					d.pathrenderer.use();
					std::for_each(d.params.begin(), d.params.end(), [&](std::pair<const std::string, float> p){
						d.pathrenderer.set(p.first,p.second);
					});
					int j = 0;
					std::for_each(d.tx.begin(), d.tx.end(), [&](std::pair<const std::string, Texture*> p){
						d.pathrenderer.bindTexture(p.first,*p.second, j++);
					});
					//Can't draw without inputs
					d.pathrenderer.set("count", (float)minsz);
					d.pathrenderer.set("time", d.time);
					d.pathrenderer.set("camera", d.camera);
					
					if(d.va.indices()) 
						d.va.drawIndices(0, 1, d.drawtype);
					else
						d.va.draw(0, minsz, 1, d.drawtype);
					
				}
			}
		}
	}
	
	void run(shiva::eventmap &received, shiva::state *s, boost::any &data) const {
		auto &d = boost::any_cast<pathdata&>(data);
		if(received.count("resourcechange")) d.re = true;
		if(received.count(d.timesrc)){
			d.time = boost::any_cast<float>(received[d.timesrc]);
		}
		if(received.count(d.camerasrc)){
			d.camera = boost::any_cast<glm::mat4>(received[d.camerasrc]);
		}
	}
};
behaviour_add(drawpath);