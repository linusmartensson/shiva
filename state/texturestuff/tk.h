#pragma once 

#include <windows.h>
#include<boost/make_shared.hpp>

#include <Shlobj.h>

#include<pcl/point_types.h>
#include<pcl/io/pcd_io.h>
#include<pcl/point_cloud.h>
#include<pcl/filters/voxel_grid.h>
#include<pcl/registration/ia_ransac.h>
#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"framebuffer.h"
#include"image.h"
#include"texture.h"
#include"midi.h"
#include"clengine.h"
#include"music.h"
#include"shaders.h"
#include"camera.h"

struct tgdata{
	bool reload;
	camera c;
	music *m;
	FrameBuffer fb, tfb;
	PNGImage gz;
	Texture color, depth, greet, tfbc, tfbd, timg, tdx, tdy;
	FSShader fss, tfss;

	Buffer<GLfloat> points;
	Buffer<GLfloat> colors;
	
	VArray va;
	Program p; 

	tgdata() : reload(false), m(new music("statedata/dq/Linus.mp3")),
		color(8, GL_RGBA32F), depth(8, GL_DEPTH_COMPONENT32F), fss("statedata/texturestuff/hdr.frag"), tfss("statedata/texturestuff/gaussian1.frag"), 
		tfbc(1, GL_RGBA32F, GL_TEXTURE_2D), tfbd(1, GL_DEPTH_COMPONENT32F, GL_TEXTURE_2D), 
		p(	"statedata/texturestuff/s2.frag", 
			"statedata/texturestuff/quad.geom", 
			"statedata/texturestuff/pt.vert"),
		gz("statedata/texturestuff/lena.png"),
		greet(1, GL_RGBA8), timg(1, GL_RGBA32F), tdx(1, GL_RGBA32F), tdy(1, GL_RGBA32F) {}
	~tgdata(){}
	void refresh(){
		reload = false;

		p.rebuild("statedata/texturestuff/s2.frag", "statedata/texturestuff/quad.geom", "statedata/texturestuff/pt.vert");

		p.bindAttrib(0, "pos");
		p.bindAttrib(1, "color");
		
		fss.rebuild("statedata/texturestuff/hdr.frag");

		greet.bind();
		glTexImage2D(GL_TEXTURE_2D_MULTISAMPLE, 0, GL_RGBA8, gz.width(), gz.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, gz.data());

		tfbc.bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gz.width(), gz.height(), 0, GL_RGBA, GL_FLOAT, 0);
		tfbd.bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, gz.width(), gz.height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

		m->time(0);
	}
};

class tk : public shiva::behaviour {

	//IO handling
	virtual void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		auto &d = boost::any_cast<tgdata&>(data);
		if(received.count("kinput")){
			std::pair<int,bool> g = boost::any_cast<std::pair<int,bool>>(received["kinput"]);
			if(g.first == GLFW_KEY_ESC)
				shiva::core::stop();			
			if(g.first == GLFW_KEY_SPACE && g.second==true){
				d.m->pause(!d.m->pause());
			}
			if(g.first == GLFW_KEY_LEFT && g.second == true){
				d.m->time(d.m->time()-1);
			}
			if(g.first == GLFW_KEY_RIGHT && g.second == true){
				d.m->time(d.m->time()+1);
			}
		}		
		if(received.count("resourcechange")){
			d.reload = true;
		}
		d.c.run(received, d.m->time(), d.m->pause());
		if(d.m->time()>122) shiva::core::stop();
	}

	virtual void render(shiva::childvec children, boost::any &data) const {
		
		auto &d = boost::any_cast<tgdata&>(data);
		
		d.fb.render([&]{
			std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();});
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
		
			if(d.reload){d.refresh();}
		
			glClearColor(0.0,0.0,0.0,1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			float speed = 0.1;
		
			d.p.use();
			glm::mat4 cam = 
					glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 500000.0f)
					*glm::mat4_cast(glm::rotate(glm::quat(), -d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.c.rot.x, glm::vec3(0,1,0)))
					*glm::translate(glm::mat4(1.0f), d.c.trans*speed*glm::vec3(-1.f,-1.f,1.f));

			speed = 0.5;
		
			d.p.set("m", cam);
			d.va.draw(0, d.gz.width()*d.gz.height(), 1, GL_POINTS);
		});
		d.fss.program().use();
		d.fss.input("greetz", d.greet);
		d.fss.input("tex", d.color);
		d.fss.set("time", d.m->time());
		d.fss.set("resolution", glm::vec2(shiva::core::width(), shiva::core::height()));
		d.fss.program().bindTexture("tex", d.color);
		d.fss.display();
	}

	virtual void init(shiva::state *instance, boost::any &data) const {
		instance->reg("kinput");
		instance->reg("resourcechange");
		auto tt = tgdata();
		data = boost::any(tt);
		auto &d = boost::any_cast<tgdata&>(data);		
		d.fb.colorbuffer(0, d.color);
		d.fb.depthbuffer(d.depth);

		d.c.init(instance, "statedata/texturestuff/0.camera");
		d.refresh();

		std::vector<glm::vec4> orig;
		orig.resize(d.gz.width()*d.gz.height());
		for(size_t y=0;y<d.gz.height(); ++y){
			for(size_t x=0;x<d.gz.width(); ++x){
				auto &b = orig[y*d.gz.width()+x];
				b.r = d.gz.data()[y*d.gz.width()*4+x*4+1]/255.0;
				b.g = d.gz.data()[y*d.gz.width()*4+x*4+2]/255.0;
				b.b = d.gz.data()[y*d.gz.width()*4+x*4+3]/255.0;
				b.a = d.gz.data()[y*d.gz.width()*4+x*4+0]/255.0;
			}
		}

		std::vector<glm::vec4> img;
		img.resize(d.gz.width()*d.gz.height());
		for(size_t y=0;y<d.gz.height(); ++y){
			for(size_t x=0;x<d.gz.width(); ++x){
				auto &b = img[y*d.gz.width()+x];
				b.r = d.gz.data()[y*d.gz.width()*4+x*4+1]/255.0;
				b.g = d.gz.data()[y*d.gz.width()*4+x*4+2]/255.0;
				b.b = d.gz.data()[y*d.gz.width()*4+x*4+3]/255.0;
				b.a = d.gz.data()[y*d.gz.width()*4+x*4+0]/255.0;
			}
		}


		d.timg.bind();
		
		glTexImage2D(d.timg.texturedims, 0, d.timg.internalformat, d.gz.width(), d.gz.height(), 0, GL_RGBA, GL_FLOAT, &img[0]);
		d.tfb.colorbuffer(0, d.tfbc);
		d.tfb.depthbuffer(d.tfbd);
		d.tfb.render([&]{
			glClearColor(1.0,1.0,1.0,1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			d.tfss.input("tex", d.timg);
			d.tfss.set("res", glm::vec2(d.gz.width(), d.gz.height()));
			d.tfss.display();
			glReadPixels(0,0,d.gz.width(), d.gz.height(), GL_RGBA, GL_FLOAT, &img[0]);
			d.tdy.bind();
			glTexImage2D(d.timg.texturedims, 0, d.timg.internalformat, d.gz.width(), d.gz.height(), 0, GL_RGBA, GL_FLOAT, &img[0]);

			d.tfss.rebuild("statedata/texturestuff/gaussian2.frag");
			d.tfss.input("tex", d.timg);
			d.tfss.set("res", glm::vec2(d.gz.width(), d.gz.height()));
			d.tfss.display();
			glReadPixels(0,0,d.gz.width(), d.gz.height(), GL_RGBA, GL_FLOAT, &img[0]);
			d.tdx.bind();
			glTexImage2D(d.timg.texturedims, 0, d.timg.internalformat, d.gz.width(), d.gz.height(), 0, GL_RGBA, GL_FLOAT, &img[0]);

			d.tfss.rebuild("statedata/texturestuff/compderivmag.frag");
			d.tfss.input("dx", d.tdx);
			d.tfss.input("dy", d.tdy);
			d.tfss.set("res", glm::vec2(d.gz.width(), d.gz.height()));
			d.tfss.display();
			glReadPixels(0,0,d.gz.width(), d.gz.height(), GL_RGBA, GL_FLOAT, &img[0]);
			d.timg.bind();
			glTexImage2D(d.timg.texturedims, 0, d.timg.internalformat, d.gz.width(), d.gz.height(), 0, GL_RGBA, GL_FLOAT, &img[0]);
			
			d.tfss.rebuild("statedata/texturestuff/thin1.frag");
			for(int i=0;i<100;++i){
				
				d.tfss.input("tex", d.timg);
				d.tfss.set("res", glm::vec2(d.gz.width(), d.gz.height()));
				d.tfss.display();
				glReadPixels(0,0,d.gz.width(), d.gz.height(), GL_RGBA, GL_FLOAT, &img[0]);
				d.timg.bind();
				glTexImage2D(d.timg.texturedims, 0, d.timg.internalformat, d.gz.width(), d.gz.height(), 0, GL_RGBA, GL_FLOAT, &img[0]);
			}
		});
		



		std::vector<glm::vec3> xyz;
		xyz.resize(d.gz.width()*d.gz.height());
		for(size_t y=0;y<d.gz.height(); ++y){
			for(size_t x=0;x<d.gz.width(); ++x){
				auto &a = xyz[y*d.gz.width()+x];
				a.x = (float)x-(float)d.gz.width()/2;
				a.y = (float)d.gz.height()/2-(float)y;
				a.z = -100.0f;
			}
		}

		d.colors.data(img.size()*4, (GLfloat*)&img[0]);
		d.points.data(xyz.size()*3, (GLfloat*)&xyz[0]);
		d.va.vertices(0, 3, d.points);
		d.va.vertices(1, 4, d.colors);
		d.m->pause(false);
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(tk);