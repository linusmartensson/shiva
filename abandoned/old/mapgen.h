#pragma once
#include"stdafx.h"
#include"state.h"
#include"clengine.h"
#include"shaders.h"
#include"cltree.h"
#include"data.h"
#include"hdr.h"
#include"camera.h"

#include"controllers.h"
#include"controllables.h"
#include"framebuffer.h"
#include"mixer.h"
#include"input.h"

using namespace shiva::controls;

class mapgen : public shiva::state {
	hdrstate *hs;
	Instance<FSShader> fs, ofs;
	Instance<FrameBuffer> fb;
	Instance<Texture> wv;
	struct Location{
		int x, y;
		int z, world;
		Location() : x(0), y(0), z(0), world(0) {}
	};
	Location location;
	int speed;
	int scw, sch;
public:
	mapgen(hdrstate *hs) : hs(hs) {
		
		speed = 5;
		onresources *ok = new onresources;
		ok->add([&]{fs->rebuild(Shader::version330() + Shader::noiselibs() + "mapgen/citycentre.frag");}, false);
		ok->add([&]{ofs->rebuild(Shader::version330() + Shader::noiselibs() + "mapgen/output.frag");}, false);
		sl.add(ok);
		fs = FSShader(Shader::version330() + Shader::noiselibs() + "mapgen/citycentre.frag");
		ofs = FSShader(Shader::version330() + Shader::noiselibs() + "mapgen/output.frag");

		sl.add(new shiva::controllable::control<int>(location.y, [&](int f, int i){return f+speed*i;}, shiva::controllable::key, 'W', 'S', true));
		sl.add(new shiva::controllable::control<int>(location.x, [&](int f, int i){return f+speed*i;}, shiva::controllable::key, 'D', 'A', true));
		sl.add(new shiva::controllable::control<int>(location.world, [&](int f, int i){return f+speed*i;}, shiva::controllable::wheel));
		sch = 16;
		scw = sch*16/9;
		wv = Texture(1, GL_RGBA, GL_TEXTURE_2D, scw, sch);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		fb = FrameBuffer();
		fb->colorbuffer(0, wv);
		fb->colorbuffer(1, wv);
	}
	virtual void reshape(int w, int h){
		sl.reshape(w, h);
	}
	virtual void kinput(int k, bool s){
		sl.kinput(k,s);
	}
	virtual void display(){
		fb->render([&]{
			fs->program().set("location", glm::vec4(location.x/(float)scw+0.5, location.y/(float)sch+0.5, location.z, location.world));
			fs->display();
		});
		ofs->set("wh", glm::vec2(scw, sch));
		ofs->input("tex", wv);
		ofs->display();
		sl.display();
	}

	virtual void update(){
		sl.update();
	}
};