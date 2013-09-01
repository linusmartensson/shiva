#pragma once 

#include <windows.h>
#include<boost/make_shared.hpp>

#include <Shlobj.h>

#include "NuiApi.h"
#include<pcl/point_types.h>
#include<pcl/filters/passthrough.h>
#include<pcl/point_cloud.h>
#include<pcl/io/file_io.h>
#include<pcl/io/pcd_io.h>
#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"framebuffer.h"
#include"texture.h"
#include"midi.h"
#include"music.h"
#include"shaders.h"
#include"camera.h"
#include"state/common/nuisensor.h"

#define POINT_ELEMCOUNT 10
#define POINT_XYZ		 0
#define POINT_NXYZ		 3
#define POINT_RGBA		 6
struct point{
	float x,y,z;
	float nx,ny,nz;
	float r,g,b,a;

	std::ostream& save(std::ostream &ofs){
		ofs<<x<<" "<<y<<" "<<z<<" ";
		ofs<<nx<<" "<<ny<<" "<<nz<<" ";
		ofs<<r<<" "<<g<<" "<<b<<" "<<a<<" ";
		return ofs;
	}
	std::istream& load(std::istream &ifs){
		ifs>>x>>y>>z;
		ifs>>nx>>ny>>nz;
		ifs>>r>>g>>b>>a;
		return ifs;
	}
};
struct framedata {
	camera c;
	std::vector<point> points, keypoints;
	Buffer<GLfloat> buffer;
	VArray va;
	framedata() : c(true) {}
	std::ostream& save(std::ostream &ofs){
		c.save(ofs);
		ofs<<points.size()<<" ";
		for(auto i=points.begin(); i!=points.end(); ++i){
			i->save(ofs);
		}
		ofs<<keypoints.size()<<" ";
		for(auto i=keypoints.begin(); i!=keypoints.end(); ++i){
			i->save(ofs);
		}
		return ofs;
	}
	std::istream& load(std::istream &ifs){
		c.init(ifs);
		
		size_t s;
		ifs>>s;
		points.reserve(s);
		points.resize(s);
		for(size_t i=0;i<s;++i){
			points[i].load(ifs);
		}
		ifs>>s;
		keypoints.reserve(s);
		keypoints.resize(s);
		for(size_t i=0;i<s;++i){
			keypoints[i].load(ifs);
		}
		return ifs;
	}
	void rebuild(){
		buffer.bind();
		buffer.data(POINT_ELEMCOUNT*points.size(), (float*)&points[0]);
		va.bind();
		va.vertices(0, 3, buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_XYZ);
		va.vertices(1, 4, buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_RGBA);	
		va.vertices(2, 3, buffer,1,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_NXYZ);
	}
};

struct tgdata{
	std::vector<framedata> frames;
	bool reload, gotdata, remove;
	camera c;
	nuicamera *ncamera;
	nuidata *ndata;
	uint frame;
	music *m;
	int active;
	int saving;

	point *b;
			
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud;
	pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr normalcloud, normals;
	bool autocollect;
	
	Buffer<GLfloat> buffer;
	VArray va;
	Program p; 

	tgdata() : reload(false), gotdata(false), remove(false), saving(0), ncamera(0), ndata(0), active(0), m(0),
		p(	"statedata/df/simple.frag", 
			"statedata/df/cube.geom", 
			"statedata/df/pt.vert") {}
	~tgdata(){}
	void refresh(){
		reload = false;
		autocollect = false;
		
		if(m!=0) delete m;
		m = new music("");

		if(ncamera==0){
			cloud = pcl::PointCloud<pcl::PointXYZRGB>::Ptr(new pcl::PointCloud<pcl::PointXYZRGB>(640,480));
			normalcloud = pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr(new pcl::PointCloud<pcl::PointXYZRGBNormal>);

			normals = pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
			ncamera = new nuicamera(true, true);
			ndata = new nuidata(ncamera);
			b = new point[400000];
		}
		active = -1;
		frame = 0;
		
		p.rebuild("statedata/df/simple.frag", "statedata/df/cube.geom", "statedata/df/pt.vert");

		p.bindAttrib(0, "pos");
		p.bindAttrib(1, "color");

	}
};

class tg : public shiva::behaviour {

	//IO handling
	virtual void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		auto &d = boost::any_cast<tgdata&>(data);
		if(received.count("kinput")){
			std::pair<int,bool> g = boost::any_cast<std::pair<int,bool>>(received["kinput"]);
			if(g.first == GLFW_KEY_ESC)
				shiva::core::stop();
			if(g.first == GLFW_KEY_SPACE && g.second==true){
				d.saving++;
				Log::info()<<"last frame:"<<d.frame<<std::endl;
				d.frame = 0;
			}
			if(g.first == GLFW_KEY_BACKSPACE){
				d.frame = 0;
			}
		}		
		if(received.count("resourcechange")){
			d.reload = true;
		}
		d.active = std::max(d.active, -1);
		d.active = std::min(d.active, (int)d.frames.size()-1);
		//Camera/Matrix
		d.c.automate = false;
		if(d.active==-1) d.c.run(received, d.m->time(), d.m->pause());
		int j=0;
		for(auto i=d.frames.begin();i!=d.frames.end();++i, ++j){
			i->c.automate = false;
			if(j==d.active) i->c.run(received, d.m->time(), d.m->pause());
		}
	}

	bool framein(pcl::PointCloud<pcl::PointXYZRGB>::Ptr &cloud, nuidata *ndata) const {
		if(ndata != 0 && ndata->peekdepth() && ndata->peekcolor()){
			
			auto dd = ndata->depthdata();
			auto cc = ndata->colordata();
			auto ci = ndata->icolormap();

			float scale = tan(58.5f * (3.14159265359f / 180.0f) * 0.5f) / (640.f * 0.5f);

			cloud->clear();
			
			for(size_t i=0;i<640*480;++i){
				auto v = dd[i];
				if(v<=1) v = std::numeric_limits<float>::quiet_NaN();
				int X = i%640;
				int Y = i/640;
				int j = (i>>1)<<2;
				int cmv = ci[j]+ci[j+1]*640;
				
				auto s = pcl::PointXYZRGB();
				s.x=(320-X)*scale*v*0.001f;
				s.y=-(Y-240)*scale*v*0.001f;
				s.z=-(v-4096)*0.001f;
				s.b=cc[cmv*4];
				s.g=cc[cmv*4+1];
				s.r=cc[cmv*4+2];
				s.a=cc[cmv*4+3];
				cloud->push_back(s);
			}
			
			pcl::PassThrough<pcl::PointXYZRGB> pt;	
			pt.setInputCloud(cloud);
			pt.filter(*cloud);

			ndata->clearcolor();
			ndata->cleardepth();
			return true;
		} 
		return false;
	}

	void toframe(pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr &points, pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr &keypoints, framedata &frame, point *b) const {
		
			for(auto s = points->begin(); s!= points->end(); ++s){
				point p;
				p.x = s->x*1000.f;
				p.y = s->y*1000.f;
				p.z = s->z*1000.f;
				p.nx = s->normal_x;
				p.ny = s->normal_y;
				p.nz = s->normal_z;
				p.r = s->r;
				p.g = s->g;
				p.b = s->b;
				p.a = s->a;
				frame.points.push_back(p);
			}
			
			for(auto s = keypoints->begin(); s!= keypoints->end(); ++s){
				point p;
				p.x = s->x*1000.f;
				p.y = s->y*1000.f;
				p.z = s->z*1000.f;
				p.nx = s->normal_x;
				p.ny = s->normal_y;
				p.nz = s->normal_z;
				p.r = s->r;
				p.g = s->g;
				p.b = s->b;
				p.a = s->a;
				frame.keypoints.push_back(p);
			}
			
			size_t q = 0;
			for(auto j = frame.points.begin(); j!=frame.points.end()&&q<400000;++j,++q){
				b[q].x = j->x;
				b[q].y = j->y;
				b[q].z = j->z;
				b[q].r = j->r;
				b[q].g = j->g;
				b[q].b = j->b;
				b[q].a = 1.f;
			}
			frame.buffer.data(POINT_ELEMCOUNT*frame.points.size(), (float*)b);
			
			q = 0;
			for(auto j = frame.keypoints.begin(); j!=frame.keypoints.end()&&q<400000;++j,++q){
				b[q].x = j->x;
				b[q].y = j->y;
				b[q].z = j->z;
				b[q].r = j->r;
				b[q].g = j->g;
				b[q].b = j->b;
				b[q].a = 1.f;
			}
			
			frame.va.bind();
			frame.va.vertices(0, 3, frame.buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_XYZ);
			frame.va.vertices(1, 4, frame.buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_RGBA);
			
	}

	virtual void render(shiva::childvec children, boost::any &data) const {
		
		auto &d = boost::any_cast<tgdata&>(data);
		std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();});
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		if(d.reload){d.refresh();}
		
		bool result = framein(d.cloud, d.ndata);
		d.gotdata |= result;
		if(result && d.cloud->size() > 0){
			auto frame = framedata();
			d.normals->clear();
			pcl::copyPointCloud(*d.cloud, *d.normalcloud);
			toframe(d.normalcloud, d.normals, frame, d.b);
			std::ostringstream oss;
			oss<<"resources/rawclouds/"<<(d.saving/2)<<"raw."<<d.frame<<".frame";
			if((d.saving+1)%2==0){
				pcl::PCDWriter pcdwriter;
				pcdwriter.write(oss.str(), *d.cloud, true);
				d.frame++;
			}
			glClearColor(0.0,0.0,0.0,1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			float speed = 0.1;
		
			d.p.use();
			glm::mat4 cam = 
					glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 500000.0f)
					*glm::mat4_cast(glm::rotate(glm::quat(), -d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.c.rot.x, glm::vec3(0,1,0)))
					*glm::translate(glm::mat4(1.0f), d.c.trans*speed*glm::vec3(-1.f,-1.f,1.f));

			speed = 0.5;
			glm::mat4 trans = 
			glm::translate(glm::mat4(1.0f), frame.c.trans*speed*glm::vec3(-1.f,-1.f,1.f))
			*glm::mat4_cast(glm::rotate(glm::quat(), -frame.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -frame.c.rot.x, glm::vec3(0,1,0)));
			
			d.p.set("m", cam*trans);
			if(frame.points.size()>0) frame.va.draw(0, frame.points.size(), 1, GL_POINTS);
		}
			

	}

	virtual void init(shiva::state *instance, boost::any &data) const {
		instance->reg("kinput");
		instance->reg("resourcechange");
		auto tt = tgdata();
		data = boost::any(tt);
		auto &d = boost::any_cast<tgdata&>(data);
		d.c.init(instance, "");
		d.refresh();
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(tg);