#pragma once
#include"stdafx.h"
#include"clengine.h"
#include"varray.h"
#include"shaders.h"
#include"framebuffer.h"
#include"fonts.h"

class fonttest : public shiva::state {
	FrameBuffer fb;
	Texture *t2;
	FSShader *sqp;
	Font f;
public:
	void display(){
		sqp->program().use();
		sqp->input("text", *t2);
		sqp->display();
	}
	bool stateUpdate(){
		return true;
	}
	void reshape(int w, int h){
		if(t2)delete sqp, delete t2;
		sqp = new FSShader("fonttest.frag");
		f.pixelsize(128);
		t2 = new Texture(f.render("Three little birds bouncing away."));
	}
	void update(){
	}
	void start(state *laststate){
	}
	void stop(state *newstate){}
	fonttest(int argc, char **argv) : 
		t2(0), sqp(0), f("exmouth/exmouth_.ttf") {
	}
	
	~fonttest(){if(t2) delete sqp, delete t2;}
};
