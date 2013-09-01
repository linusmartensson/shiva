#pragma once
#include"stdafx.h"
#include"clengine.h"
#include"varray.h"
#include"shaders.h"
#include"framebuffer.h"

class squareplane : public shiva::state {
	FrameBuffer fb;
	Texture *t;
	FSShader *fss, *sqp;
public:
	void display(){
		sqp->program().use();
		glUniform1f(sqp->program().uniform("time"), shiva::core::renderTime());

		fb.colorbuffer(0, *t);
		fb.render(sqp);
		
		fss->input("tex", *t);
		fss->display();
	}
	bool stateUpdate(){
		return true;
	}
	void reshape(int w, int h){
		if(t)delete t,delete fss, delete sqp;
		fss = new FSShader("hdr.frag");
		sqp = new FSShader("squareplane.frag");
		t = new Texture(8, GL_RGB16F);
		fss->program().use();
		glUniform1f(fss->program().uniform("maxbright"), 40.0f);
		glUniform1f(fss->program().uniform("exposure"), 1.0f);
	}
	void update(){
	}
	void start(state *laststate){
	}
	void stop(state *newstate){}
	squareplane(int argc, char **argv) : 
		t(0), fss(0), sqp(0) {
	}
	
	~squareplane(){if(t)delete t,delete fss, delete sqp;}
};
