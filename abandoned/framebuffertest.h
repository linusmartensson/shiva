#pragma once
#include"stdafx.h"
#include"varray.h"
#include"state.h"
#include"shaders.h"
#include"tests/particles.h"
#include"tests/lines.h"
#include"framebuffer.h"


class FrameBufferTest : public shiva::state{
	state *nb;
	Instance<Texture> t, t2;
	Instance<FSShader> fss, fss2, fss3;
	FrameBuffer fb;
	
public:
	virtual void update(){nb->update();}
	virtual bool stateUpdate(){return nb->stateUpdate();}
	virtual void start(shiva::state *laststate){nb->start(laststate);}
	virtual void stop(shiva::state *newstate){nb->stop(newstate);}
	virtual void reshape(int w, int h){
		nb->reshape(w,h);
		t = Texture(8, GL_R16F), t2 = Texture(8, GL_R16F);
		fss = FSShader("drawtest.frag");
		fss2 = FSShader("guassianvert.frag");
		fss3 = FSShader("guassianhoriz.frag");
		fss->program().use();
		glUniform1f(fss->program().uniform("maxbright"), 40.0f);
		glUniform1f(fss->program().uniform("exposure"), 1.0f);
	}
	void blur(){
		fb.colorbuffer(0, *t);
		fss2->input("tex", t2);
		fb.render((FSShader *)fss2);

		fb.colorbuffer(0, *t2);
		fss3->input("tex", t);
		fb.render((FSShader *)fss3);
	}
	virtual void display(){
		fb.colorbuffer(0, *t2);
		fb.render(nb);
		
		fss->input("tex", t2);
		fss->display();
	}
	FrameBufferTest(int argc, char** argv) : nb(new particles(argc, argv)) {}
	~FrameBufferTest(){
		delete nb;
	}
};