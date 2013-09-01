#pragma once 

#include <windows.h>
#include<boost/make_shared.hpp>

#include <Shlobj.h>

#include "NuiApi.h"
#include<pcl/point_types.h>
#include<pcl/features/normal_3d.h>
#include<pcl/filters/extract_indices.h>
#include<pcl/registration/icp.h>
#include<pcl/filters/passthrough.h>
#include<pcl/filters/statistical_outlier_removal.h>
#include<pcl/keypoints/sift_keypoint.h>
#include<pcl/features/fpfh.h>
#include<pcl/registration/registration.h>
#include<pcl/point_cloud.h>
#include<pcl/filters/approximate_voxel_grid.h>
#include<pcl/surface/mls.h>
#include<pcl/surface/marching_cubes_hoppe.h>
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
		ofs<<x<<" "<<y<<" "<<z<<std::endl;
		ofs<<nx<<" "<<ny<<" "<<nz<<std::endl;
		ofs<<r<<" "<<g<<" "<<b<<" "<<a<<std::endl;
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
	//std::ostream& save(std::ostream &ofs){
	//	c.save(ofs);
	//	ofs<<fragname<<std::endl<<geoname<<std::endl<<vertname<<std::endl;
	//	ofs<<points.size()<<" ";
	//	for(auto i=points.begin(); i!=points.end(); ++i){
	//		i->save(ofs);
	//	}
	//	ofs<<keypoints.size()<<" ";
	//	for(auto i=keypoints.begin(); i!=keypoints.end(); ++i){
	//		i->save(ofs);
	//	}
	//	return ofs;
	//}
	//std::istream& load(std::istream &ifs){
	//	c.init(ifs);
	//	std::getline(ifs, fragname);
	//	std::getline(ifs, geoname);
	//	std::getline(ifs, vertname);
	//	
	//	size_t s;
	//	ifs>>s;
	//	points.reserve(s);
	//	points.resize(s);
	//	for(size_t i=0;i<s;++i){
	//		points[i].load(ifs);
	//	}
	//	ifs>>s;
	//	keypoints.reserve(s);
	//	keypoints.resize(s);
	//	for(size_t i=0;i<s;++i){
	//		keypoints[i].load(ifs);
	//	}
	//	return ifs;
	//}
	void rebuild(){
		buffer.bind();
		buffer.data(POINT_ELEMCOUNT*points.size(), (float*)&points[0]);
		va.bind();
		va.vertices(0, 3, buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_XYZ);
		va.vertices(1, 4, buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_RGBA);	
		va.vertices(2, 3, buffer,1,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_NXYZ);
	}
};
class PRep : public pcl::PointRepresentation<pcl::PointXYZRGBNormal> {
	using pcl::PointRepresentation<pcl::PointXYZRGBNormal>::nr_dimensions_;
public:
	PRep(){nr_dimensions_ = 4;}

	virtual void copyToFloatArray(const pcl::PointXYZRGBNormal &p, float *out) const {
		out[0] = p.x;
		out[1] = p.y;
		out[2] = p.z;
		out[3] = p.curvature;
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

	point *b;
			
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud;
	pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr normalcloud, normals;
	bool autocollect;
	
	Buffer<GLfloat> buffer;
	VArray va;
	Program p; 

	tgdata() : reload(false), gotdata(false), remove(false), ncamera(0), ndata(0), active(0), m(0),
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
				d.frame++;
			}
			if(g.first == GLFW_KEY_BACKSPACE){
				d.frame = 0;
			}
			if(g.first == 'Y' && g.second==true){
				d.autocollect = !d.autocollect;
			}
			if(g.first == 'T' && g.second==true){
				d.frame = 0;
				d.gotdata = false;
			}
			if(g.first == '9' && g.second==true){
				d.active--;
			}
			if(g.first == '0' && g.second==true){
				d.active++;
			}
		}		
		if(received.count("resourcechange")){
			d.reload = true;
		}
		d.active = std::max(d.active, -1);
		d.active = std::min(d.active, (int)d.frames.size()-1);
		//Camera/Matrix
		Log::info()<<d.active<<std::endl;
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

	void tokeypoints(pcl::PointCloud<pcl::PointXYZRGB>::Ptr &cloud, pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr &out) const {
			pcl::PointCloud<pcl::PointXYZRGB>::Ptr ci (new pcl::PointCloud<pcl::PointXYZRGB>());
			pcl::PointCloud<pcl::PointXYZRGB>::Ptr ci2 (new pcl::PointCloud<pcl::PointXYZRGB>());
			
			pcl::ApproximateVoxelGrid<pcl::PointXYZRGB> vgrid;
			
			float idensity = 0.02f;
			vgrid.setLeafSize(idensity,idensity,idensity);
			vgrid.setInputCloud(cloud);
			vgrid.filter(*ci);
			
			pcl::SIFTKeypoint<pcl::PointXYZRGB, pcl::PointXYZRGB> sift;
			sift.setMinimumContrast(1);
			sift.setScales(0.005, 2, 5);
			sift.setInputCloud(ci);
			sift.compute(*ci2);
			
			pcl::NormalEstimation<pcl::PointXYZRGB, pcl::PointXYZRGBNormal> ne;
			ne.setRadiusSearch(0.2);
			ne.setInputCloud(ci2);
			ne.compute(*out);
			pcl::copyPointCloud(*ci2, *out);
	}
	
	void normals(pcl::PointCloud<pcl::PointXYZRGB>::Ptr &cloud, pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr &out) const {
		pcl::NormalEstimation<pcl::PointXYZRGB, pcl::PointXYZRGBNormal> ne;
		ne.setRadiusSearch(0.04);
		ne.setInputCloud(cloud);
		ne.compute(*out);
		pcl::copyPointCloud(*cloud, *out);			
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
		
		if(d.frame%2==0){
			bool result = framein(d.cloud, d.ndata);
			d.gotdata |= result;
		}
		if(d.frame%2==1){
			d.frame++;
			
			Log::info()<<"Collected data"<<std::endl;
			normals(d.cloud, d.normalcloud);		
			Log::info()<<"Processed normals"<<std::endl;	
			tokeypoints(d.cloud, d.normals);
			Log::info()<<"Processed keypoints"<<std::endl;
			d.frames.push_back(framedata());
			toframe(d.normalcloud, d.normals, d.frames.back(), d.b);
			Log::info()<<"Built frame"<<std::endl;
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
		for(auto i = d.frames.begin(); i!=d.frames.end(); ++i){
			glm::mat4 trans = 
				glm::translate(glm::mat4(1.0f), i->c.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::mat4_cast(glm::rotate(glm::quat(), -i->c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -i->c.rot.x, glm::vec3(0,1,0)));
			
			d.p.set("m", cam*trans);
			if(i->points.size()>0) i->va.draw(0, i->points.size(), 1, GL_POINTS);
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