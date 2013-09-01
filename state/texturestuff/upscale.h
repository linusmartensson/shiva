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

struct point{
	union{float r; float m00;};
	union{float g; float m01;};
	union{float b; float m10;};
	union{float a; float m11;};
	point operator*(const point &q)const {
		point p = {r*q.r,g*q.g,b*q.b, a*q.a};
		return p;
	}
	point operator*(const float &f)const {
		point p = {r*f,g*f,b*f, a*f};
		return p;
	}
	point operator/(const point &q)const {
		point p = {r/q.r,g/q.g,b/q.b, a/q.a};
		return p;
	}
	point operator/(const float &f)const {
		point p = {r/f,g/f,b/f, a/f};
		return p;
	}
	point operator+(const point &q)const {
		point p = {r+q.r,g+q.g,b+q.b, a+q.a};
		return p;
	}
	point operator-(const point &q)const {
		point p = {r-q.r,g-q.g,b-q.b, a-q.a};
		return p;
	}
	point &operator=(const point &q){
		r = q.r;
		g = q.g;
		b = q.b;
		a = q.a;
		return *this;
	}
};

struct covpoint{
	point r;
	point g;
	point b;
	point a;
};

struct tgdata{
	bool reload;
	FSShader fss;

	VArray va;
	Buffer<GLfloat> b;
	Buffer<GLuint> indices;
	point *img, *gauss, *mean;
	point *cmoment11, *cmoment20, *cmoment02;
	point *moment00, *moment10, *moment01, *moment20, *moment02, *moment11;
	point *det, *detmin, *detmax, *beta;
	covpoint *cpt;
	PNGImage image;
	Texture t, t2, t3, t4;

	tgdata() : indices(GL_ELEMENT_ARRAY_BUFFER),
				 reload(false), 
				 image("statedata/texturestuff/monarch.png"),
 				 fss("statedata/texturestuff/simple.frag"), t(1, GL_RGBA32F, GL_TEXTURE_2D), t2(1, GL_RGBA32F, GL_TEXTURE_2D), t3(1, GL_RGBA32F, GL_TEXTURE_2D), t4(1, GL_RGBA32F, GL_TEXTURE_2D), img(0) {
	}
	~tgdata(){
	}

	float gaussian(int x, int y, float o){
		o *= o*2.f;
		float e = 2.71828f;
		float p = o*3.14159265;
		return pow(e, -(x*x+y*y)/o)/p;
	}
	point moment(int ix, int iy, point *p, int w, int h, int o, int i, int j){
		point moment = {0,0,0,0};
		for(int y=(iy-o*3);y<(iy+o*3);++y){
			for(int x=(ix-o*3);x<(ix+o*3);++x){
				
				//diff from start point.
				float oy = y-(iy-o*3)+1;
				float ox = x-(ix-o*3)+1;

				//Handle corners.
				int yy = y;
				int xx = x;
				yy = abs(yy);
				xx = abs(xx);
				if(yy>=h-1) yy = h-(yy%(h-1))-1;
				if(xx>=w-1) xx = w-(xx%(w-1))-1;

				//get target point.
				point *q = p+yy*w+xx;

				moment.r += pow(ox,i)*pow(oy,j)*q->r;
				moment.g += pow(ox,i)*pow(oy,j)*q->g;
				moment.b += pow(ox,i)*pow(oy,j)*q->b;
				moment.a += pow(ox,i)*pow(oy,j)*q->a;
			}
		}
		return moment;
	}
	point meankernel(int ix, int iy, point *p, int w, int h, int o){
		point mean = {0,0,0,0};
		int qq=0;
		for(int y=(iy-o*3);y<(iy+o*3);++y){
			for(int x=(ix-o*3);x<(ix+o*3);++x){
				
				//diff from center.
				int oy = y-iy;
				int ox = x-ix;

				//Handle corners.
				int yy = y;
				int xx = x;
				yy = abs(yy);
				xx = abs(xx);
				if(yy>=h-1) yy = h-(yy%(h-1))-1;
				if(xx>=w-1) xx = w-(xx%(w-1))-1;

				//get target point.
				point *q = p+yy*w+xx;

				mean.r += q->r;
				mean.g += q->g;
				mean.b += q->b;
				mean.a += q->a;
				qq++;
			}
		}
		mean.r /= qq;
		mean.g /= qq;
		mean.b /= qq;
		mean.a /= qq;
		return mean;		
	}
	void normalkernel(int ix, int iy, point *p, int w, int h, int o, point &b00, point &b10, point &b11){

		b00.r=b00.g=b00.b=b00.a=0;
		b10.r=b10.g=b10.b=b10.a=0;
		b11.r=b11.g=b11.b=b11.a=0;

		//Centerpoint
		point *c = p+iy*w+ix;

		for(int y=(iy-o*3);y<=(iy+o*3);++y){
			for(int x=(ix-o*3);x<=(ix+o*3);++x){
				
				//diff from center.
				int oy = y-iy;
				int ox = x-ix;

				//Handle corners.
				int yy = y;
				int xx = x;
				yy = abs(yy);
				xx = abs(xx);
				if(yy>=h-1) yy = h-(yy%(h-1))-1;
				if(xx>=w-1) xx = w-(xx%(w-1))-1;

				//get target point.
				point *q = p+yy*w+xx;

				float ax = x-ix;
				float ay = y-iy;

				float a00 = ax*ax;
				float a10 = ax*ay;
				float a11 = ay*ay;

				float Z = sqrt(ax*ax+ay*ay);
				point fac = (*q-*c)*(*q-*c)/(Z*Z)/((o*3*2+1)*(o*3*2+1));
				
				b00 = b00+fac*a00;
				b10 = b10+fac*a10;
				b11 = b11+fac*a11;
			}
		}
	}
	point gaussiankernel(int ix, int iy, point *p, int w, int h, int o){
		point gauss = {0,0,0,0};
		for(int y=(iy-o*3);y<(iy+o*3);++y){
			for(int x=(ix-o*3);x<(ix+o*3);++x){
				
				//diff from center.
				int oy = y-iy;
				int ox = x-ix;

				//Handle corners.
				int yy = y;
				int xx = x;
				yy = abs(yy);
				xx = abs(xx);
				if(yy>=h-1) yy = h-(yy%(h-1))-1;
				if(xx>=w-1) xx = w-(xx%(w-1))-1;

				//get target point.
				point *q = p+yy*w+xx;

				float g = gaussian(ox,oy,o);

				gauss.r += g*q->r;
				gauss.g += g*q->g;
				gauss.b += g*q->b;
				gauss.a += g*q->a;
			}
		}
		
		return gauss;
	}

	void minmaxkernel(int ix, int iy, point *p, int w, int h, int o, point &min, point &max){
		min = max = *p;
		for(int y=(iy-o*3);y<(iy+o*3);++y){
			for(int x=(ix-o*3);x<(ix+o*3);++x){
				
				//diff from center.
				int oy = y-iy;
				int ox = x-ix;

				//Handle corners.
				int yy = y;
				int xx = x;
				yy = abs(yy);
				xx = abs(xx);
				if(yy>=h-1) yy = h-(yy%(h-1))-1;
				if(xx>=w-1) xx = w-(xx%(w-1))-1;

				//get target point.
				point *q = p+yy*w+xx;

				min.r = q->r<min.r?q->r:min.r;
				min.g = q->g<min.r?q->g:min.g;
				min.b = q->b<min.r?q->b:min.b;
				min.a = q->a<min.r?q->a:min.a;
				max.r = q->r>max.r?q->r:max.r;
				max.g = q->g>max.g?q->g:max.g;
				max.b = q->b>max.b?q->b:max.b;
				max.a = q->a>max.a?q->a:max.a;
			}
		}
		
	}

	void refresh(){
		
		reload = false;
		image.rebuild();
		
		if(img==0){
			img = new point[image.width()*image.height()];
			gauss = new point[image.width()*image.height()];
			mean = new point[image.width()*image.height()];
			
			beta = new point[image.width()*image.height()];
			det = new point[image.width()*image.height()];
			detmin = new point[image.width()*image.height()];
			detmax = new point[image.width()*image.height()];
			

			for(int y=0;y<image.height();++y){
				for(int x=0;x<image.width();++x){
					
					img[y*image.width()+x].r = image.data()[y*image.width()*4+x*4]/255.f;
					img[y*image.width()+x].g = image.data()[y*image.width()*4+x*4+1]/255.f;
					img[y*image.width()+x].b = image.data()[y*image.width()*4+x*4+2]/255.f;
					img[y*image.width()+x].a = image.data()[y*image.width()*4+x*4+3]/255.f;
				}
			}
			for(int y=0;y<image.height();++y){
				for(int x=0;x<image.width();++x){
					gauss[y*image.width()+x] = gaussiankernel(x,y, img, image.width(), image.height(), 1);
					point b00, b10, b11;
					normalkernel(x,y,img,image.width(),image.height(),1, b00, b10, b11);
				}
			}
			
			for(int y=0;y<image.height();++y){
				for(int x=0;x<image.width();++x){
					auto m00 = moment(x,y, gauss, image.width(), image.height(), 1,0,0);
					auto m10 = moment(x,y, gauss, image.width(), image.height(), 1,1,0);
					auto m01 = moment(x,y, gauss, image.width(), image.height(), 1,0,1);
					auto m11 = moment(x,y, gauss, image.width(), image.height(), 1,1,1);
					auto m20 = moment(x,y, gauss, image.width(), image.height(), 1,2,0);
					auto m02 = moment(x,y, gauss, image.width(), image.height(), 1,0,2);
					auto cm20 = m20/m00 - m10*m10/(m00*m00);
					auto cm02 = m02/m00 - m01*m01/(m00*m00);
					auto cm11 = m11/m00 - m10*m01/(m00*m00);

					int t = y*image.width()+x;
					det[t] = cm20*cm02-cm11*cm11;
				}
			}
			
			//multi-pixel op needs separate pass.
			for(int y=0;y<image.height();++y){
				for(int x=0;x<image.width();++x){
					int t = y*image.width()+x;
					
					point m, M;
					minmaxkernel(x,y, det, image.width(),image.height(), 1, m, M);

					beta[t] = (det[t]-m)/(M-m);
				}
			}
			
		}
		
		t.bind();
		glTexImage2D(t.texturedims, 0, t.internalformat, image.width(), image.height(), 0, GL_RGBA, GL_FLOAT, beta);
		/*t2.bind();
		glTexImage2D(t2.texturedims, 0, t.internalformat, image.width(), image.height(), 0, GL_RGBA, GL_FLOAT, detmax);
		t3.bind();
		glTexImage2D(t3.texturedims, 0, t.internalformat, image.width(), image.height(), 0, GL_RGBA, GL_FLOAT, det);*/
		t4.bind();
		glTexImage2D(t4.texturedims, 0, t.internalformat, image.width(), image.height(), 0, GL_RGBA, GL_FLOAT, img);
		fss.rebuild("statedata/texturestuff/simple.frag");
		fss.input("beta", t);
		//fss.input("detmax", t2);
		//fss.input("det", t3);
		fss.input("img", t4);
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
		
		d.fss.display();
	}

	virtual void init(shiva::state *instance, boost::any &data) const {
		instance->reg("kinput");
		instance->reg("resourcechange");
		auto tt = tgdata();
		data = boost::any(tt);
		auto &d = boost::any_cast<tgdata&>(data);
		d.refresh();
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(tg);