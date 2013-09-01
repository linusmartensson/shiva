#pragma once

#include<dirent.h>
#include"state.h"
#include"fonts.h"
#include"shaders.h"
/*
class list : public shiva::state {
	std::function<size_t()> esize;
	std::function<std::string(int)> ename;
	std::function<void(int)> erun;

	size_t active;
	int sz;
	font *f;
	int w, h;
	FSShader rectShader;
public:
	list() : f(0), w(1), h(1), active(0), rectShader("editor/generator/rect.frag") {}
	~list(){delete f;}

	template<typename A, typename B, typename C>
	void setup(A esize, B ename, C erun, int sz=24){
		this->esize = esize; 
		this->ename = ename;
		this->erun = erun;
		this->sz = sz;

		this->f = new font("resources/fonts/monof55.ttf", sz);
	}


	void rect(int w, int h, int x, int y, int ow, int oh, bool curr){
		rectShader.clear(false);
		float oho = h/float(oh);
		float owo = w/float(ow);
		rectShader.transform(glm::mat4(
			owo			,0				,0				,0,
			0			,oho			,0				,0,
			0			,0				,1				,0,
			-1.0f+owo+2*x/float(ow)		,-1.0f+oho+2*y/float(oh) ,0	,1
		));
		rectShader.set("active", curr?1.f:0.f);
		rectShader.display();
	}

	void display(){
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT);
		int j=0;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		for(size_t j=0;j!=esize(); ++j){
			int p = esize()-1-j;
			int x = w/2-ename(j).size()*f->width()/2;
			int y = h/2-esize()*sz/2+(p*sz)%(h-10);
			f->setPenPosition(x,y);
			f->setPenColor(active==j?1.f:0.f,1.f,1.f);
			rect(w/2,sz,w/4,y,w,h,j==active);
			f->draw(ename(j));
		}
	}
	void reshape(int w, int h){
		this->w = w; this->h=h;
		f->setDisplaySize(w,h);
	}
	void kinput(int k, bool s){
		if(!s) return;
		if(k==GLFW_KEY_DOWN)	active = (active+1)%esize();
		if(k==GLFW_KEY_UP)		active = active==0?esize()-1:active-1;
		if(k==GLFW_KEY_ENTER)	erun(active%esize());
	}
};*/