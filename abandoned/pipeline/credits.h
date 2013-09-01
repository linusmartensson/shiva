#pragma once
#include"stdafx.h"
#include"state.h"
#include"cltree.h"
#include"shaders.h"
#include"misc.h"
#include"framebuffer.h"
#include"fonts.h"
#include"controllables.h"
#include<functional>

using namespace shiva;


class credits : public shiva::state {
	std::vector<Instance<Texture>> text;
	Font f;
	FSShader fss;
public:
	credits() : f("fonts/TECHNOID.TTF"), fss("statedata/pipeline/particular/outrofont.frag")
	{	
		f.pixelsize(200);
		
		onresources *or = new onresources;
		or->add([&]{
			fss.rebuild("statedata/pipeline/particular/outrofont.frag");
		}, false);
		sl.add(or);
		
		text.push_back(Instance<Texture>(f.render("nalle")));
		text.push_back(Instance<Texture>(f.render("w0lfis")));
		text.push_back(Instance<Texture>(f.render("Kalaspuff")));
		text.push_back(Instance<Texture>(f.render("Radd0x")));
		text.push_back(Instance<Texture>(f.render("devvis")));
		text.push_back(Instance<Texture>(f.render("TMC")));
		text.push_back(Instance<Texture>(f.render("Topatisen")));
		text.push_back(Instance<Texture>(f.render("Candela")));
		text.push_back(Instance<Texture>(f.render("Jimmy")));
		text.push_back(Instance<Texture>(f.render("Mental")));
	}
	virtual void display(){
		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		sl.display();
		int j=0;
		fss.set("time", core::renderTime());
		for(auto i=text.begin(); i!=text.end(); ++i, ++j){
			fss.input("text", **i);
			fss.set("offset", (float)j);
			fss.set("names", (float)text.size());
			fss.clear(false);
			fss.display();
		}
	}
};