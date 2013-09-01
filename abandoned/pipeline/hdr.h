#pragma once
#include"stdafx.h"
#include"framebuffer.h"
#include"shaders.h"
#include"state.h"
#include"postprocessor.h"
#include"input.h"

struct hdrstate{
	virtual ~hdrstate(){}
};	

class hdrpipe : public shiva::state, public hdrstate{
	
	shiva::statelist *rsl;
	
	Instance<Texture> auxtexture, aux2texture, hdrtexture, depthtexture, sdepthtexture, bdepthtexture, outtexture;
	Instance<FSShader> output;
	int w, h;
public:
	
	void restart(){
		output->rebuild("statedata/pipeline/hdr/output.frag");
	}

	hdrpipe(){
		rsl = new shiva::statelist();
		
		output = FSShader("statedata/pipeline/hdr/output.frag");


		sl.add((new onkey('Q'))->add([&]{restart();}, false));
		sl.add((new onresources)->add([&]{restart();}, true));
		
		sl.add(&(new fbpass([&](FrameBuffer &f, int w, int h){
			f.colorbuffer(0, hdrtexture);
			f.depthbuffer(depthtexture);
		}, [&](){
		}))->add(rsl));

		sl.add(new rpass([&](int w, int h){

		}, [&]{
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			
			output->input("tex", hdrtexture);
			output->set("time", shiva::core::renderTime());
			output->set("res", glm::vec2(w,h));
			output->display();
		}));
	}
	
	void reshape(int w, int h){
		this->w = w;
		this->h = h;
		hdrtexture = Texture(8, GL_RGB16F);
		depthtexture = Texture(8, GL_DEPTH_COMPONENT24);

		return sl.reshape(w,h);
	}

	hdrpipe& add(state *s){rsl->add(s); return *this;}
	public:	
};