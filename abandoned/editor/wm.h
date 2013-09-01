#pragma once
#include"shaders.h"
#include"shiva.h"
#include"postprocessor.h"
/*
class windowmanager : public shiva::state { 

	FSShader surfaceShader;
	bool changed;
	
	int cx, cy;
	size_t active;

	bool flip;
	bool fullscreen;

	std::set<int> downkeys;

	std::map<std::pair<int,int>, Instance<Texture>> textures;
	std::map<std::pair<int,int>, Instance<Texture>> depths;
	
	shiva::state *wrap(shiva::state *s){
		auto fb = new fbpass();
		fb->setup([&](FrameBuffer &fb, int w, int h){
			if(!textures.count(std::make_pair(w,h))){
				textures[std::make_pair(w,h)] = Texture(1, GL_RGBA8, GL_TEXTURE_2D, w, h);
				depths[std::make_pair(w,h)] = Texture(1, GL_DEPTH_COMPONENT24, GL_TEXTURE_2D, w, h);
			}
			fb.colorbuffer(0, textures[std::make_pair(w,h)]);
			fb.depthbuffer(depths[std::make_pair(w,h)]);
		}, [&]{});
   		fb->add(s);
		return fb;
	}

	std::function<shiva::state* (windowmanager *m)> creator;

	bool delayedReorder;

public:

	windowmanager() : surfaceShader("editor/wm/wm.frag"), delayedReorder(false), changed(false), cx(0), cy(0), active(0), fullscreen(false), flip(false) {}

	template<typename T>
	void listbuilder(T t){
		creator = t;
	}

	~windowmanager(){
	}

	void reorder(bool full = false){
		fullscreen = full;
		runTranslated([&](state *i, int ow, int oh, int w, int h, int x, int y, int pos){
			i->reshape(w, h);
		});
	}

	windowmanager& add(shiva::state *s){
		sl.insert(sl.begin()+active, wrap(s));
		reorder(fullscreen);
		return *this;
	}

	windowmanager& replace(shiva::state *s){
		sl.insert(sl.begin()+active, wrap(s));
		if(sl.size()>0){
			auto ss = sl[active+1];
			del.push_back(ss);
		}
		delayedReorder=true;
		return *this;
	}

	template<typename T>
	void runTranslated(T &f){
		
		int w=shiva::core::width(),h=shiva::core::height(),x=0,y=0;
		
		if(fullscreen && active < sl.size()){
			f(sl[active], w,h,w,h,x,y,active);
			return;
		}

		bool splitw=flip;
		size_t j=0;
		for(auto i=sl.begin();i!=sl.end();++i,++j){
			splitw = !splitw;
			if(j<sl.size()-1){
				if(splitw){
					w /= 2;
					//vertical split line
				} else {
					h /= 2;
					//horizontal split line
				}
			}

			f(*i, shiva::core::width(), shiva::core::height(), w,h, x,y, j);

			if(splitw){
				x += w;
			} else {
				y += h;
			}
		}
	}
	void display(){
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT);
		
		runTranslated([&](state *i, int ow, int oh, int w, int h, int x, int y, int pos){			
			surfaceShader.clear(false);

			float oho = h/float(oh);
			float owo = w/float(ow);

			surfaceShader.transform(glm::mat4(
				owo			,0				,0				,0,
				0			,oho			,0				,0,
				0			,0				,1				,0,
				-1.0f+owo+2*x/float(ow)		,-1.0f+oho+2*y/float(oh)		,0				,1
			));

			i->display();
			surfaceShader.input("surfaceTexture", textures[std::make_pair(w,h)]);
			surfaceShader.set("active", pos==active?1.f:0.f);
			surfaceShader.display();
		});
	}

	//Input functions are only run on the current surface. A complete up/down chain of events is not guaranteed as of now.
	virtual void winput(bool up){
		if(sl.size() <= active) return;
		sl[active]->winput(up);
	}
	virtual void binput(int btn, bool state){
		if(sl.size() <= active) return;
		sl[active]->binput(btn, state);
	}
	virtual void minput(int ox, int oy){
		cx = ox; cy = oy;
		runTranslated([&](shiva::state *i, int ow, int oh, int w, int h, int x, int y, int pos){
			if(pos!=active) return;
			i->minput(ox-x, oy-y);}
		);
	}
	virtual void kinput(int k, bool s){
		if(downkeys.count(k) && !s){
			downkeys.erase(k);
			return;
		}
		if(shiva::core::down(GLFW_KEY_LALT) && s){
			downkeys.insert(k);
			if(k==GLFW_KEY_ENTER){
				add(creator(this));
			}
			if(shiva::core::down(GLFW_KEY_LSHIFT)){
				if(k=='J' && sl.size()>1){
					int curr = active;
					int next = active==0?sl.size()-1:active-1;
					
					auto temp = sl[curr];
					sl[curr] = sl[next];
					sl[next] = temp;
					
					active = next;

					reorder();
				}
				if(k=='K' && sl.size()>1){
					int curr = active;
					int next = (active+1)%sl.size();
					
					auto temp = sl[curr];
					sl[curr] = sl[next];
					sl[next] = temp;
					
					active = next;

					reorder();
				}
				if(k=='F'){
					flip = !flip;
					reorder(fullscreen);
				}
			} else {
				if(k=='K' && sl.size()>0){
					active = (active+1)%sl.size();
					if(fullscreen) reorder(true);
				}
				if(k=='J' && sl.size()>0){
					active = active==0?sl.size()-1:active-1;
					if(fullscreen) reorder(true);
				}
				if(k=='Q'){
					if(sl.size()>0){
						auto o = *(sl.begin()+active);
						del.push_back(o);
						if(active == sl.size()-1 && sl.size()-1 > 0) active--;
						delayedReorder = true;
					}
				}
				if(k=='F'){
					reorder(!fullscreen);
				}
			}
			return;
		}

		if(active >= sl.size()) return;
		sl[active]->kinput(k, s);
	}

	//Reshape is mapped elsewhere and should be run whenever items are reordered or changed, not here.
	virtual void reshape(int w, int h){
		textures.clear();
		depths.clear();
		reorder();
	}

	bool stateUpdate(){
		bool ret = shiva::state::stateUpdate();
		if(delayedReorder){
			reorder(fullscreen);
			ret = true;
			delayedReorder=false;
		}
		return ret;
	}
};*/