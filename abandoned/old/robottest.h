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
#include<time.h>

using namespace shiva::controls;

class robottest : public shiva::state {
	hdrstate *hs;
	float speed;
	float rot;
	float x, y, ex, ey, px, py;
	Instance<Buffer<GLfloat>> buf, ibuf;
	Instance<VArray> vb;
	Instance<Program> p;
	std::vector<GLfloat> f;
public:
	robottest(hdrstate *hs) : hs(hs) {
		p = Program("robottest/pr.frag", "passthrough.geom", "passthrough.vert");
		speed = 0.0f;
		rot = 0.f;
		x=y=ex=ey=px=py=0.0f;
		f = std::vector<GLfloat>(9, 0.f);
		sl.add(new shiva::controllable::control<float>(speed, [&](float f, int i){return f+0.001f*i;}, shiva::controllable::key, 'W', 'S', true));
		sl.add(new shiva::controllable::control<float>(rot, [&](float f, int i){return f-0.1f*i;}, shiva::controllable::key, 'D', 'A', true));
		
		buf = Buffer<GLfloat>(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY); 
		ibuf = Buffer<GLfloat>(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY); 
		buf->data(f);
		vb = VArray();
		srand(time(0));
	}
	virtual bool stateUpdate(){
		sl.stateUpdate();
		
		ibuf->cloneContent(*buf);
		Log::GLErrors("cloneContent");

		return true;
	}
	float rp(float mag){
		return ((rand()%10000)/5000.f)*mag-mag;
	}
	virtual void update(){
		x += cos(rot)*speed;
		y += sin(rot)*speed;
		ex += cos(rot+rp(1))*(speed);
		ey += sin(rot+rp(1))*(speed);
		px = px*0.8 + ex*0.2;
		py = py*0.8 + ey*0.2;
		f[0] = x;
		f[1] = y;
		f[3] = ex;
		f[4] = ey;
		f[6] = px;
		f[7] = py;
		buf->data(f);
		sl.update();
	}
	virtual void display(){
		glEnable(GL_BLEND);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE,GL_ONE);
		
		p->use();
		p->set("mat", glm::mat4(1.0f));
		p->bindAttrib(0, "pos");
		vb->vertices(0, 3, *ibuf);
		vb->draw(0, ibuf->size(), 1, GL_POINTS);

		sl.display();
	}

}; 