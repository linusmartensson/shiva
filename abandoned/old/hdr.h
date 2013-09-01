#pragma once
#include"stdafx.h"
#include"framebuffer.h"
#include"shaders.h"
#include"state.h"
#include"postprocessor.h"

struct hdrstate{
	virtual void toneMapping(float brightnessCap, float exposure) = 0;
	virtual void bloomblur(float vertical, float horizontal) = 0;
	virtual void accblur(float vertical, float horizontal) = 0;
	virtual void mix(float current, float acc, float bloom) = 0;
	virtual void acc(float current, float acc, float bloom) = 0;
	virtual float& exposure() = 0;
	virtual float& brightnessCap() = 0;
	virtual float& bloomVertical() = 0;
	virtual float& bloomHorizontal() = 0;
	virtual float& accblurVertical() = 0;
	virtual float& accblurHorizontal() = 0;
	virtual float& mixCurrent() = 0;
	virtual float& mixOld() = 0;
	virtual float& mixBloom() = 0;
	virtual float& accCurrent() = 0;
	virtual float& accOld() = 0;
	virtual float& accBloom() = 0;
	virtual void clearacc() = 0;
	virtual ~hdrstate(){}
};	

class hdrpipe : public shiva::state, public hdrstate{
	
	shiva::statelist *rsl;
	
	Instance<Texture> auxtexture, aux2texture, hdrtexture, depthtexture, sdepthtexture, bdepthtexture, acctexture, acc2texture, outtexture;
	Instance<FSShader> hdrrender, blurvert, blurhoriz, accupdate, output;

	Instance<FrameBuffer> fbclearacc, fbclearacc2;

	shiva::statelist clear;

	float br, exp, bloomv, bloomh, accv, acch, mcur, macc, mbloom, acur, aacc, abloom;
public:

	hdrpipe(){
		br = 1.0, exp = 1.0, bloomv = 1.0, bloomh = 1.0, accv = 0.0, acch = 0.0, mcur = 1.0, macc = 0.0, mbloom = 1.0, acur = 0.0, aacc = 0.0, abloom = 0.0;
		rsl = new shiva::statelist();
		
		hdrrender = FSShader("tools/hdr/hdr.frag");
		blurvert = FSShader("tools/hdr/guassianvert.frag");
		blurhoriz = FSShader("tools/hdr/guassianhoriz.frag");
		accupdate = FSShader("tools/hdr/accupdate.frag");
		output = FSShader("tools/hdr/output.frag");

		sl.add(&(new fbpass([&](FrameBuffer &f, int w, int h){
			f.colorbuffer(0, hdrtexture);
			f.depthbuffer(depthtexture);
		}, [&](){
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
		}))->add(rsl));

		sl.add(new fbpass([&](FrameBuffer &f, int w, int h){
			f.colorbuffer(0, auxtexture);
			f.depthbuffer(bdepthtexture);
		}, [&](){
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			blurvert->input("tex", hdrtexture);
			blurvert->set("blur", bloomv);
			blurvert->set("resolution", glm::vec2(shiva::core::width(), shiva::core::height()));
			blurvert->display();
		}));

		sl.add(new fbpass([&](FrameBuffer &f, int w, int h){
			f.colorbuffer(0, aux2texture);
			f.depthbuffer(bdepthtexture);
		}, [&](){
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			blurhoriz->set("blur", bloomh);
			blurhoriz->input("tex", auxtexture);
			blurhoriz->set("resolution", glm::vec2(shiva::core::width(), shiva::core::height()));
			blurhoriz->display();
		}));
		sl.add(new fbpass([&](FrameBuffer &f, int w, int h){
			f.colorbuffer(0, outtexture);
			f.colorbuffer(1, acctexture);
			f.depthbuffer(sdepthtexture);
		}, [&](){
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			hdrrender->input("tex", hdrtexture);
			hdrrender->input("blur", aux2texture);
			hdrrender->input("acc", acc2texture);
			hdrrender->input("depth", depthtexture);
			
			hdrrender->set("maxbright", br);
			hdrrender->set("exposure", exposure());

			hdrrender->set("accnew", acur);
			hdrrender->set("accbloom", abloom);
			hdrrender->set("accold", aacc);
			
			hdrrender->set("mixnew", mcur);
			hdrrender->set("mixbloom", mbloom);
			hdrrender->set("mixold", macc);

			hdrrender->display();
		}));
		sl.add(new fbpass([&](FrameBuffer &f, int w, int h){
			f.colorbuffer(0, auxtexture);
			f.depthbuffer(bdepthtexture);
		}, [&](){
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			blurvert->set("blur", accv);
			blurvert->input("tex", acctexture);
			blurvert->display();
		}));
		sl.add(new fbpass([&](FrameBuffer &f, int w, int h){
			f.colorbuffer(0, aux2texture);
			f.depthbuffer(bdepthtexture);
		}, [&](){
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			blurhoriz->set("blur", acch);
			blurhoriz->input("tex", auxtexture);
			blurhoriz->display();
		}));
		sl.add(new fbpass([&](FrameBuffer &f, int w, int h){
			f.colorbuffer(0, acc2texture);
			f.depthbuffer(sdepthtexture);
		}, [&](){
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			accupdate->input("tex", aux2texture);
			accupdate->display();
		}));
		sl.add(new rpass([&](int w, int h){

		}, [&]{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			
			output->input("tex", outtexture);
			output->display();
		}));

		clear.add(new fbpass([&](FrameBuffer &f, int w, int h){
			f.colorbuffer(0, acctexture);
			f.depthbuffer(sdepthtexture);
		}, [&]{
			glClearColor(0,0,0,1);
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}));
		clear.add(new fbpass([&](FrameBuffer &f, int w, int h){
			f.colorbuffer(0, acc2texture);
			f.depthbuffer(sdepthtexture);
		}, [&]{
			glClearColor(0,0,0,1);
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}));
	}
	
	void reshape(int w, int h){

		hdrtexture = Texture(8, GL_RGB16F);
		depthtexture = Texture(8, GL_DEPTH_COMPONENT24);
		auxtexture = Texture(1, GL_RGB16F, GL_TEXTURE_2D);
		aux2texture = Texture(1, GL_RGB16F, GL_TEXTURE_2D);
		bdepthtexture = Texture(1, GL_DEPTH_COMPONENT24, GL_TEXTURE_2D);
		sdepthtexture = Texture(1, GL_DEPTH_COMPONENT24);
		acctexture = Texture(1, GL_RGB16F);
		acc2texture = Texture(1, GL_RGB16F);
		outtexture = Texture(1, GL_RGB16F);

		clear.reshape(w,h);	

		return sl.reshape(w,h);
	}

	void kinput(int k, bool s){	
		if(!s && k == GLFW_KEY_HOME) hdrrender->rebuild("tools/hdr/hdr.frag");
		sl.kinput(k,s);
	}

	hdrpipe& add(state *s){rsl->add(s); return *this;}
	public:	
	virtual float& exposure() {return exp;}
	virtual float& brightnessCap() {return br;}
	virtual float& bloomVertical() {return bloomv;}
	virtual float& bloomHorizontal() {return bloomh;}
	virtual float& accblurVertical() {return accv;}
	virtual float& accblurHorizontal() {return acch;}
	virtual float& mixCurrent() {return mcur;}
	virtual float& mixOld() {return macc;}
	virtual float& mixBloom() {return mbloom;}
	virtual float& accCurrent() {return acur;}
	virtual float& accOld() {return aacc;}
	virtual float& accBloom() {return abloom;}
	virtual void toneMapping(float brightnessCap, float exposure){
		br = brightnessCap;
		exp = exposure;
	}
	virtual void bloomblur(float vertical, float horizontal){
		bloomv = vertical;
		bloomh = horizontal;
	}
	virtual void accblur(float vertical, float horizontal){
		accv = vertical;
		acch = horizontal;
	}
	virtual void mix(float current, float acc, float bloom){
		mcur = current;
		macc = acc;
		mbloom = bloom;
	}
	virtual void acc(float current, float acc, float bloom){
		acur = current;
		aacc = acc;
		abloom = bloom;
	}
	virtual void clearacc(){
		clear.display();
	}
	
};