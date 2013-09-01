#pragma once 

#include <windows.h>
#include<boost/make_shared.hpp>

#include<png.h>
#include <Shlobj.h>

#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"framebuffer.h"
#include"texture.h"
#include"shaders.h"
#include"image.h"

struct tgdata{
	bool reload;
	FSShader fss;

	VArray va;
	Buffer<GLfloat> b;
	Buffer<GLuint> indices;
	glm::vec4 *img;
	Program p;
	PNGImage image;
	Texture t;

	tgdata() : indices(GL_ELEMENT_ARRAY_BUFFER),
				 reload(false), 
				 image("statedata/texturestuff/monarch.png"),
 				 fss("statedata/texturestuff/simple.frag"), t(1, GL_RGBA32F, GL_TEXTURE_2D), img(0) {
	}
	~tgdata(){
	}


	void refresh(){
		
		reload = false;
		image.rebuild();
		
		if(img==0){
			img = new glm::vec4[image.width()*image.height()];
			for(size_t y=0;y<image.height();++y){
				for(size_t x=0;x<image.width();++x){
					
					img[y*image.width()+x].r = image.data()[y*image.width()*4+x*4]/255.f;
					img[y*image.width()+x].g = image.data()[y*image.width()*4+x*4+1]/255.f;
					img[y*image.width()+x].b = image.data()[y*image.width()*4+x*4+2]/255.f;
					img[y*image.width()+x].a = image.data()[y*image.width()*4+x*4+3]/255.f;
				}
			}
		}
	}
};

class tg : public shiva::behaviour {

	virtual void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		auto &d = boost::any_cast<tgdata&>(data);
		if(received.count("kinput")){
			std::pair<int,bool> g = boost::any_cast<std::pair<int,bool>>(received["kinput"]);
			if(g.first == GLFW_KEY_ESC)
				shiva::core::stop();
		}		
		if(received.count("resourcechange")){
			d.reload = true;
		}
	}

	virtual void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<tgdata&>(data);
		std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();});
		
		if(d.reload){
			d.refresh();
		}
	
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
		float speed = 0.1f;
		
		glTexImage2D(d.t.texturedims, 0, d.t.internalformat, d.image.width(), d.image.height(), 0, GL_RGBA, GL_FLOAT, d.img);
		d.fss.rebuild("statedata/texturestuff/out.frag");
		d.fss.input("img", d.t);
		d.fss.display();


		d.va.vertices(0, 3, d.b); 
		d.va.indices(d.indices);
		d.va.drawIndices(0,1,GL_POINTS);

	}

	virtual void init(shiva::state *instance, boost::any &data) const {
		instance->reg("kinput");
		instance->reg("resourcechange");
		auto tt = tgdata();
		data = boost::any(tt);
		auto &d = boost::any_cast<tgdata&>(data);
		d.refresh();

		std::vector<glm::vec3> xyz;
		xyz.resize(d.image.width()*d.image.height());
		for(size_t y=0;y<d.image.height(); ++y){
			for(size_t x=0;x<d.image.width(); ++x){
				auto &a = xyz[y*d.image.width()+x];
				a.x = x;
				a.y = y;
				a.z = z;
			}
		}

//		d.b.data(

	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(tg);