#pragma once 

#include <windows.h>
#include<boost/make_shared.hpp>

#include <Shlobj.h>

#include "NuiApi.h"
#include<pcl/point_types.h>
#include<pcl/features/normal_3d.h>
#include<pcl/features/integral_image_normal.h>
#include<pcl/features/integral_image2D.h>
#include<pcl/filters/extract_indices.h>
#include<pcl/registration/icp_nl.h>
#include<pcl/registration/icp.h>
#include<pcl/registration/ia_ransac.h>
#include<pcl/filters/passthrough.h>
#include<pcl/filters/statistical_outlier_removal.h>
#include<pcl/keypoints/sift_keypoint.h>
#include<pcl/features/fpfh.h>
#include<pcl/registration/registration.h>
#include<pcl/point_cloud.h>
#include<pcl/filters/voxel_grid.h>
#include<pcl/filters/approximate_voxel_grid.h>
#include<pcl/surface/gp3.h>
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

struct point{
	float x,y,z;
	float r,g,b,a;
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
	bool reload, gotdata;
	camera c;
	Program p;
	nuicamera *ncamera;
	nuidata *ndata;
	uchar *color;
	uint frame;
	
	pcl::PolygonMesh triangles;
	VArray va;
	Buffer<GLfloat> b;
	Buffer<GLuint> indices;
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud,rendercloud;
	pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr normals,normals_tgt;
	pcl::PointCloud<pcl::FPFHSignature33>::Ptr features, features_tgt;
	point *tbb;
	Eigen::Matrix<float, 4,4, Eigen::DontAlign> m, curr;
	float lim;
	int delay;
	bool autocollect;
	int rcchanged;

	tgdata() : indices(GL_ELEMENT_ARRAY_BUFFER),
				 reload(false), gotdata(false), ncamera(0), ndata(0), rcchanged(false), 
 				 p("statedata/df/simple.frag", "statedata/df/cube.geom", "statedata/df/pt.vert"){
	}
	~tgdata(){
	}
	void refresh(){
		Log::info()<<"tgdata refresh"<<std::endl;
		reload = false;
		autocollect = false;
		if(ncamera==0){
			delay = 0;
			cloud = pcl::PointCloud<pcl::PointXYZRGB>::Ptr(new pcl::PointCloud<pcl::PointXYZRGB>(640,480));
			normals = pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
			normals_tgt = pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
			rendercloud = pcl::PointCloud<pcl::PointXYZRGB>::Ptr(new pcl::PointCloud<pcl::PointXYZRGB>);	
			features = pcl::PointCloud<pcl::FPFHSignature33>::Ptr(new pcl::PointCloud<pcl::FPFHSignature33>);	
			features_tgt = pcl::PointCloud<pcl::FPFHSignature33>::Ptr(new pcl::PointCloud<pcl::FPFHSignature33>);
			ncamera = new nuicamera(true, true);
			ndata = new nuidata(ncamera);
			tbb = new point[10000000];
			rcchanged = 0;
		}
		
		lim = 0.1f;
		curr = m = Eigen::Matrix4f::Identity();
		frame = 0;
		p.rebuild("statedata/df/simple.frag", "statedata/df/cube.geom", "statedata/df/pt.vert");
		
		int i=0;
		c.rebuild();
		
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
				d.rendercloud->clear();
				d.frame = 0;
				d.curr = d.m = Eigen::Matrix4f::Identity();
				d.gotdata = false;
				d.normals_tgt->clear();
				d.features_tgt->clear();
			}
		}		
		if(received.count("resourcechange")){
			d.reload = true;
		}
		d.c.run(received, 0, false);
	}

	//Point depth correction
	float getPoint(float *dd, size_t i) const {
		auto v = dd[i];
		auto vnh = 0.f;
		if(v>1) return v;
		return std::numeric_limits<float>::quiet_NaN();
	}

	virtual void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<tgdata&>(data);
		std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();});
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		if(d.reload){
			d.refresh();
		}

		float scale = tanf(NUI_CAMERA_DEPTH_NOMINAL_HORIZONTAL_FOV * (3.14159265359f / 180.0f) * 0.5f) / ( 0.5f) / 800.0f;
		
		if(d.ndata != 0 && d.ndata->peekdepth() && d.ndata->peekcolor() && (d.frame%4==0) ){
			if(d.autocollect){
				d.frame++;
			}
			d.m = d.curr;
			d.gotdata = true;
			auto dd = d.ndata->depthdata();
			auto cc = d.ndata->colordata();
			auto ci = d.ndata->icolormap();

			float scale = tan(58.5f * (3.14159265359f / 180.0f) * 0.5f) / (640.f * 0.5f);

			
			d.cloud->clear();
			
			for(size_t i=0;i<640*480;++i){
				auto v = getPoint(dd, i);
				int X = i%640;
				int Y = i/640;
				int j = (i>>1)<<2;
				int cmv = ci[j]+ci[j+1]*640;
				
				auto s = pcl::PointXYZRGB();
				s.x=(X-320)*scale*v*0.001f;
				s.y=-(Y-240)*scale*v*0.001f;
				s.z=-v*0.001f;
				s.b=(float)cc[cmv*4];
				s.g=(float)cc[cmv*4+1];
				s.r=(float)cc[cmv*4+2];
				s.a=(float)cc[cmv*4+3];
				d.cloud->push_back(s);

			}	
			
			
			pcl::PassThrough<pcl::PointXYZRGB> pt;	
			pt.setInputCloud(d.cloud);
			pt.filter(*d.cloud);

			d.ndata->clearcolor();
			d.ndata->cleardepth();
		} else if(d.frame%4==1){
			d.frame++;
				
			Log::info()<<"built cloud"<<std::endl;

			pcl::PointCloud<pcl::PointXYZRGB>::Ptr ci (new pcl::PointCloud<pcl::PointXYZRGB>());
			pcl::PointCloud<pcl::PointXYZRGB>::Ptr ci2 (new pcl::PointCloud<pcl::PointXYZRGB>());
			pcl::ApproximateVoxelGrid<pcl::PointXYZRGB> vgrid;
			float idensity = 0.03f;
			vgrid.setLeafSize(idensity,idensity,idensity);
			vgrid.setInputCloud(d.cloud);
			vgrid.filter(*ci);
			Log::info()<<"downsampled"<<std::endl;
			Log::info()<<"ci size: "<<ci->size()<<std::endl;
			
			/*

			improves performance, no noticable quality diff.

			pcl::StatisticalOutlierRemoval<pcl::PointXYZRGB> sor;
			sor.setMeanK(50);
			sor.setStddevMulThresh(1.0);
			sor.setInputCloud(ci);
			sor.filter(*ci);
			Log::info()<<"removed outliers"<<std::endl;
			Log::info()<<"ci size: "<<ci->size()<<std::endl;*/
		
			//TODO: Replace or augment this keypoint detector with an edge detector on the 2D input data.
			pcl::SIFTKeypoint<pcl::PointXYZRGB, pcl::PointXYZRGB> sift;
			sift.setMinimumContrast(1);
			sift.setScales(0.005, 2, 5);
			sift.setInputCloud(ci);
			sift.compute(*ci2);
			Log::info()<<"extracted keypoints"<<std::endl;
			Log::info()<<"ci2 size: "<<ci2->size()<<std::endl;
			
			pcl::NormalEstimation<pcl::PointXYZRGB, pcl::PointXYZRGBNormal> ne;
			ne.setRadiusSearch(0.2);
			ne.setInputCloud(ci2);
			ne.compute(*d.normals);
			pcl::copyPointCloud(*ci2, *d.normals);
			Log::info()<<"got normals"<<std::endl;
			Log::info()<<"normals size: "<<d.normals->size()<<std::endl;

			pcl::FPFHEstimation<pcl::PointXYZRGBNormal, pcl::PointXYZRGBNormal> fpf;
			fpf.setRadiusSearch(1.0);	
			fpf.setInputCloud(d.normals);
			fpf.setInputNormals(d.normals);
			fpf.compute(*d.features);
			Log::info()<<"got features"<<std::endl;
			Log::info()<<"features size: "<<d.features->size()<<std::endl;
			
			if(d.rendercloud->size() == 0){
				//First frame, nothing to map against.
				d.frame = 3;
			}else{
				d.lim = 0.3f;
				pcl::SampleConsensusInitialAlignment<pcl::PointXYZRGBNormal, pcl::PointXYZRGBNormal, pcl::FPFHSignature33> scia;
				scia.setInputCloud(d.normals);
				scia.setRANSACIterations(20);
				scia.setRANSACOutlierRejectionThreshold(0.1f);
				scia.setInputTarget(d.normals_tgt);
				scia.setMaxCorrespondenceDistance(0.1f);
				scia.setMinSampleDistance(0.2f);
				scia.setMaximumIterations(50);
				scia.setSourceFeatures(d.features);
				scia.setTargetFeatures(d.features_tgt);
			
				pcl::PointCloud<pcl::PointXYZRGBNormal> ia;
				scia.align(ia, d.m);

				Log::info()<<"ran scia"<<std::endl;

				d.m = scia.getFinalTransformation();

				pcl::IterativeClosestPoint<pcl::PointXYZRGBNormal, pcl::PointXYZRGBNormal> p;

				PRep prep;
				float alpha[4] = {1.0,1.0,1.0,1.0};
				prep.setRescaleValues(alpha);

				p.setPointRepresentation(boost::make_shared<const PRep>(prep));

				p.setRANSACIterations(20);
				p.setRANSACOutlierRejectionThreshold(0.1f);
				p.setMaximumIterations(30);
				p.setTransformationEpsilon(0.00001f);
			
				p.setInputCloud(d.normals);
				p.setInputTarget(d.normals_tgt);

				pcl::PointCloud<pcl::PointXYZRGBNormal> out;

				for(int i=0;i<5;++i){
					p.setMaxCorrespondenceDistance(d.lim);				
					p.align(out, d.m);
					if(abs(((p.getLastIncrementalTransformation()-d.m).sum()) < p.getTransformationEpsilon())){
						d.lim = d.lim*0.9;
					}	
					d.m=p.getFinalTransformation();
				}
				Log::info()<<"icp done"<<std::endl;
				Log::info()<<p.getFinalTransformation()<<std::endl;
			}
			
		} else if(d.frame%4==2){
			if(d.autocollect) d.frame++;
		} else if(d.frame%4 == 3){
			pcl::transformPointCloud(*d.cloud, *d.cloud, d.m);
			pcl::transformPointCloudWithNormals(*d.normals, *d.normals, d.m);
			
			d.curr = d.m;
			
			int a = 0;
			for(auto s = d.cloud->begin(); s!= d.cloud->end(); ++s){
				d.rendercloud->push_back(*s);
				a++;
			}
			Log::info()<<"added "<<a<<" d.rendercloud points. Downsampling..."<<std::endl;

			pcl::ApproximateVoxelGrid<pcl::PointXYZRGB> vgrid;
			vgrid.setLeafSize(0.07f,0.07f,0.07f);
			vgrid.setInputCloud(d.rendercloud);
			vgrid.filter(*d.rendercloud);
			Log::info()<<"d.rendercloud is now "<<a<<" points"<<std::endl;


			a=0;
			for(auto s = d.normals->begin(); s!= d.normals->end(); ++s){
				d.normals_tgt->push_back(*s);
				a++;
			}
			Log::info()<<"added "<<a<<" d.normals_tgt points"<<std::endl;
			a=0;
			for(auto s = d.features->begin(); s!= d.features->end(); ++s){
				d.features_tgt->push_back(*s);
				a++;
			}
			Log::info()<<"added "<<a<<" d.features_tgt points"<<std::endl;
			d.frame++;
		
			//Downsample the normal & feature clouds to avoid overly abundant amounts of features.
			
			pcl::ExtractIndices<pcl::FPFHSignature33> ei;
			ei.setNegative(true);
			ei.setIndices(pcl::IndicesPtr(new std::vector<int>));
			ei.setInputCloud(d.features_tgt);
			ei.filter(*d.features);
			d.features_tgt->clear();
			for(auto s = d.features->begin(); s!= d.features->end(); ++s){
				d.features_tgt->push_back(*s);
				a++;
			}
		}
		


		float speed = 0.1;
			
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
		int szi = 0;
		int j=0;
		if(d.rendercloud->size() > 0 && d.rcchanged != d.rendercloud->size()){
			d.rcchanged = d.rendercloud->size();

			for(auto i = d.rendercloud->begin(); i!=d.rendercloud->end() && j<10000000; ++i, ++j){
				d.tbb[j].x = i->x*1000.f;
				d.tbb[j].y = i->y*1000.f;
				d.tbb[j].z = i->z*1000.f;
				d.tbb[j].r = i->r;
				d.tbb[j].g = i->g;
				d.tbb[j].b = i->b;
				d.tbb[j].a = 0.5f;
			}

			for(auto i = d.normals_tgt->begin(); i!=d.normals_tgt->end() && j<10000000; ++i, ++j){
				d.tbb[j].x = i->x*1000.f;
				d.tbb[j].y = i->y*1000.f;
				d.tbb[j].z = i->z*1000.f;
				d.tbb[j].r = 255;
				d.tbb[j].g = 0;
				d.tbb[j].b = 0;
				d.tbb[j].a = 1.f;
			}
		}

		j = d.rendercloud->size()+d.normals_tgt->size();

		pcl::PointCloud<pcl::PointXYZRGB>::Ptr tcloud(new pcl::PointCloud<pcl::PointXYZRGB>);
		for(auto s = d.cloud->begin(); s!= d.cloud->end(); ++s){
			tcloud->push_back(*s);
		}
		pcl::transformPointCloud(*tcloud, *tcloud, d.m);
		
		for(auto i = tcloud->begin(); i!=tcloud->end() && j<10000000; ++i, ++j){
			d.tbb[j].x = i->x*1000.f;
			d.tbb[j].y = i->y*1000.f;
			d.tbb[j].z = i->z*1000.f;
			d.tbb[j].r = i->r;
			d.tbb[j].g = i->g;
			d.tbb[j].b = i->b;
			d.tbb[j].a = 0.5f;
		}


		d.b.data(j*7, (float*)d.tbb); 

		if(!d.gotdata) return;

		d.va.vertices(0, 3, d.b,0,sizeof(GLfloat)*7,0);
		d.va.vertices(1, 4, d.b,0,sizeof(GLfloat)*7,3);
		
		d.p.use();
				
		glm::mat4 cam = 
				glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 500000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.c.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.c.trans*speed*glm::vec3(-1.f,-1.f,1.f));
		d.p.set("m", cam);
		
		d.va.draw(0,j,1,GL_POINTS);
	}

	virtual void init(shiva::state *instance, boost::any &data) const {
		instance->reg("kinput");
		instance->reg("resourcechange");
		auto tt = tgdata();
		data = boost::any(tt);
		Log::info()<<"after tgdata constructor"<<std::endl;
		auto &d = boost::any_cast<tgdata&>(data);
		d.c.init(instance, "");
		d.refresh();
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(tg);