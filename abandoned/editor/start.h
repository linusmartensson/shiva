#pragma once
#include"stdafx.h"
#include"controllers.h"
#include"data.h"
#include"state.h"
#include"texture.h"
#include"state.h"
#include"postprocessor.h"
#include"state/common/music.h"
#include"shaders.h"
#include<string>
#include<set>
#include"fonts.h"
#include"wm.h"
#include"ui/list.h"
#include"projectstart.h"

/*
class writer : public shiva::behaviour {
	font f;
	int i;
	int w, h;
public:
	writer() : f("resources/fonts/monof56.ttf", 24), i(0), w(1), h(1) {
		
	}
	void display(){
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT);
		std::ostringstream oss;
		oss<<"hello! "<<(i++);
		f.setDisplaySize(w, h);
		f.setPenPosition(0,0);
		f.setPenColor(0,1,1);
		f.draw("Hello text?");
		f.setPenPosition(0,60);
		f.draw(oss.str());
		BufferBase::reset();
		Program::reset();
		VArray::reset();
	}
	void reshape(int w, int h){
		this->w = w; this->h=h;
	}
};


class test : public shiva::behaviour {
	float r,g,b;
public:

	test() : r(1), g(1), b(1){}
	void color(float r, float g, float b) { this->r=r;this->g=g;this->b=b; }
	void display(){
		glClearColor(r,g,b,1);
		glClear(GL_COLOR_BUFFER_BIT);
	}
};

class shisys : public shiva::behaviour {
	std::vector<std::pair<std::string, std::function<void(windowmanager *m)>>> wms;
	windowmanager *wm;
	std::string name;

public:

	shisys() {
		wm = new windowmanager();
		wm->listbuilder([&](windowmanager *m) -> shiva::state* {
			std::vector<std::pair<std::string, std::function<void(windowmanager *m)>>> &a = wmstates();
			list *l =new list();
			l->setup(
				[&]() -> int {
					return a.size();
				}, [&](int i){
					return a[i].first;
				},
				[&,m](int i) {
					a[i].second(m);
				});
			return l;
		});
		add.push_back(wm);
	}

	template<typename T>
	void wmstate(std::string s){
		wms.push_back(std::make_pair(s, [&](windowmanager *m){
			T* t = new T();
			m->replace(t);
		}));
	}
	std::vector<std::pair<std::string, std::function<void(windowmanager *m)>>>& wmstates(){
		return wms;
	}
	void init(std::string name){
		this->name = name;
	}
};

class start : public shiva::state {

	shiva::state *startup;
	
	shisys *sys;
	std::string statefile;

	void loadproject(std::string s){
		del.push_back(startup);
		startup = 0;
		add.push_back(sys = new shisys());

		sys->init(s);
	}

public:
	
	start() : startup(0) {
		//projectstart displays a menu and picks a project directory, creating the directory if necessary.
		projectstart *p;
		add.push_back(startup = p = new projectstart());
		p->setup([&](std::string s){loadproject(s);});
	}

	void kinput(int k, bool s){
		if(k==GLFW_KEY_F4 && shiva::core::down(GLFW_KEY_LALT)){
			shiva::core::stop();
		}
		shiva::state::kinput(k,s);
	}
};*/
