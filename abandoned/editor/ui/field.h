#pragma once

#pragma once

#include<dirent.h>
#include"state.h"
#include"fonts.h"
#include"shaders.h"
/*
class field : public shiva::state{
	std::function<void(std::string)> erun;

	int sz;
	font *f;
	int w, h;
	size_t pos;
	FSShader rectShader;
	std::string text;
public:
	field() : f(0), w(1), h(1), pos(0), rectShader("editor/generator/rect.frag") {}
	~field(){delete f;}

	template<typename C>
	void setup(C erun, int sz=24){
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
		int x = w/2-text.size()*f->width()/2;
		int y = h/2-sz/2;
		f->setPenPosition(x,y);
		f->setPenColor(1.f,1.f,1.f);
		rect(f->width(),sz,x+pos*f->width(),y,w,h,false);
		f->draw(text);
	}
	void reshape(int w, int h){
		this->w = w; this->h=h;
		f->setDisplaySize(w,h);
	}
	void kinput(int k, bool s){
		if(!s) return;
		if(k==GLFW_KEY_LEFT)	pos = pos>0?pos-1:0;
		if(k==GLFW_KEY_RIGHT)	pos = pos<text.length()?pos+1:pos;
		if(k==GLFW_KEY_ENTER)	erun(text);
		if(k==GLFW_KEY_BACKSPACE && pos>0){
			text.erase(text.begin()+--pos);
		}
	}
	void cinput(int k, bool s){
		text.insert(text.begin()+pos++,k);
	}
};*/