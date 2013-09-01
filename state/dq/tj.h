#pragma once 

#include <windows.h>
#include<boost/make_shared.hpp>

#include <Shlobj.h>

#include "NuiApi.h"
#include<pcl/point_types.h>
#include<pcl/io/pcd_io.h>
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
#include<pcl/registration/ia_ransac.h>
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

#define POINT_ELEMCOUNT 10
#define POINT_XYZ		 0
#define POINT_NXYZ		 3
#define POINT_RGBA		 6
struct point{
	float x,y,z;
	float nx,ny,nz;
	float r,g,b,a;
};
struct framedata {
	pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr points, keypoints;
	Eigen::Matrix<float, 4,4, Eigen::DontAlign> trans;
	framedata() : 
		points(new pcl::PointCloud<pcl::PointXYZRGBNormal>), 
		keypoints(new pcl::PointCloud<pcl::PointXYZRGBNormal>), 
		trans(Eigen::Matrix<float, 4,4, Eigen::DontAlign>::Identity()) 
	{}

	void rebuild(VArray &va, Buffer<GLfloat> &buffer){
			auto b = new point[points->size()];
			size_t q = 0;
			for(auto j = points->begin(); j!=points->end();++j,++q){
				b[q].x = j->x*1000.f;
				b[q].y = j->y*1000.f;
				b[q].z = j->z*1000.f;
				b[q].r = j->r;
				b[q].g = j->g;
				b[q].b = j->b;
				b[q].a = 1.f;
			}
			buffer.data(POINT_ELEMCOUNT*points->size(), (float*)b);
			va.bind();
			va.vertices(0, 3, buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_XYZ);
			va.vertices(1, 4, buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_RGBA);
			delete [] b;
	}

};

struct tgdata{
	std::vector<framedata> frames;
	bool reload, gotdata, remove;
	camera c;
	uint frame;
	music *m;
	int set;
	int mode;
	int state;
	int saving;
	bool deleted;
	bool autocollect;
	nuicamera *ncamera;
	nuidata *ndata;

	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud;
	pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr normalcloud, normals, kpts, fcloud;
	
	
	Buffer<GLfloat> buffer, buf2;
	VArray va, vb;
	Program p; 

	tgdata() : ncamera(0), ndata(0), reload(false), gotdata(false), remove(false), m(0), set(0), mode(0), state(0), saving(0),
		p(	"statedata/df/simple.frag", 
			"statedata/df/quad.geom", 
			"statedata/df/pt.vert") {}
	~tgdata(){}
	void refresh(){
		reload = false;
		deleted  =false;
		autocollect = false;
		
		if(ncamera == 0){
			ncamera = new nuicamera(true, true);
			ndata = new nuidata(ncamera);
		}

		if(m!=0) delete m;
		m = new music("");
		cloud = pcl::PointCloud<pcl::PointXYZRGB>::Ptr(new pcl::PointCloud<pcl::PointXYZRGB>(640,480));
		normalcloud = pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
		kpts = pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
		normals = pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
		fcloud = pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
		frame = 0;
		
		p.rebuild("statedata/df/simple.frag", "statedata/df/quad.geom", "statedata/df/pt.vert");

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
			if(g.first == 'B' && g.second == true){
				if(d.state == 0) d.state = 1;
				else if(d.state == 1) d.state = 0;
				d.frame = 0;
			}
			if(d.state == 0 ){
				if(g.first == GLFW_KEY_SPACE && g.second==true){
					d.saving++;
					Log::info()<<"last frame:"<<d.frame<<std::endl;
					d.frame = 0;
				}
			}
			if(d.state == 1){
				if(g.first == GLFW_KEY_ESC)
					shiva::core::stop();
				if(g.first == GLFW_KEY_SPACE && g.second==true){
					d.frame++;
				}
				if(g.first == GLFW_KEY_BACKSPACE && g.second == true && d.deleted == false){
					d.frames.pop_back();
					d.deleted = true;
				}
				if(g.first == 'Y' && g.second==true){
					d.autocollect = !d.autocollect;
				}
				if(g.first == GLFW_KEY_UP && g.second==true){
					d.set++;
					d.frame = 0;
					d.frames.clear();
					d.kpts->clear();
					d.fcloud->clear();
					d.mode = 0;
					Log::info()<<"current: "<<d.set<<std::endl;
				}
				if(g.first == GLFW_KEY_DOWN && g.second==true){
					d.set--;
					d.frame = 0;
					d.frames.clear();
					d.kpts->clear();
					d.fcloud->clear();
					d.mode = 0;
					Log::info()<<"current: "<<d.set<<std::endl;
				}
				if(g.first == 'T' && g.second==true){
					pcl::PCDWriter wrt;
					std::ostringstream fname;
					fname<<"resources/mergedclouds/"<<d.set<<".merge";
					wrt.write(fname.str(), *d.fcloud, true);
				}
				if(g.first == 'I' && g.second==true){
					d.mode=0;
					d.gotdata = false;
				}

			}
			
			
		}		
		if(received.count("resourcechange")){
			d.reload = true;
		}
		d.c.automate = false;
		d.c.run(received, 0, false);
		
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

	void toframe(pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr &points, pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr &keypoints, framedata &frame) const {
		
			*frame.points += *points;
			*frame.keypoints += *keypoints; 
	}

	virtual void render(shiva::childvec children, boost::any &data) const {
		
		auto &d = boost::any_cast<tgdata&>(data);
		std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();});
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		if(d.reload){d.refresh();}
		
		if(d.state==0){
			bool result = framein(d.cloud, d.ndata);
			d.gotdata |= result;
			if(result && d.cloud->size() > 0){
				auto frame = framedata();
				std::ostringstream oss;
				oss<<"resources/rawclouds/"<<(d.saving/2)<<"raw."<<d.frame<<".frame";
				if((d.saving+1)%2==0){
					pcl::PCDWriter pcdwriter;
					pcdwriter.write(oss.str(), *d.cloud, true);
					d.frame++;
				}
				glClearColor(0.0,0.0,0.0,1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				auto b = new point[d.cloud->size()];
				size_t q = 0;
				for(auto j = d.cloud->begin(); j!=d.cloud->end();++j,++q){
					b[q].x = j->x*1000.f;
					b[q].y = j->y*1000.f;
					b[q].z = j->z*1000.f;
					b[q].r = j->r;
					b[q].g = j->g;
					b[q].b = j->b;
					b[q].a = 1.f;
				}
				d.buffer.data(POINT_ELEMCOUNT*d.cloud->size(), (float*)b);
				d.va.bind();
				d.va.vertices(0, 3, d.buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_XYZ);
				d.va.vertices(1, 4, d.buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_RGBA);
				delete [] b;

				float speed = 0.1;
		
				d.p.use();
				glm::mat4 cam = 
						glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 500000.0f)
						*glm::mat4_cast(glm::rotate(glm::quat(), -d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.c.rot.x, glm::vec3(0,1,0)))
						*glm::translate(glm::mat4(1.0f), d.c.trans*speed*glm::vec3(-1.f,-1.f,1.f));

				d.p.set("m", cam);
				d.va.draw(0, d.cloud->size(), 1, GL_POINTS);
			}
		}
		if(d.state == 1){
			if(d.mode%2==0){
			
				if(d.frames.size() > 0 && d.deleted == false){
				
					if(d.frames.size() > 1){
						d.frames[d.frames.size()-2].keypoints->clear();
						d.frames[d.frames.size()-2].points->clear();
					}

					*d.kpts += *d.frames.back().keypoints;
					*d.fcloud += *d.frames.back().points;

					pcl::ApproximateVoxelGrid<pcl::PointXYZRGBNormal> avg;
					float dst = 0.05;
					avg.setLeafSize(dst,dst,dst);
					avg.setDownsampleAllData(true);
					avg.setInputCloud(d.fcloud);
					avg.filter(*d.fcloud);

					auto b = new point[d.fcloud->size()];
					size_t q = 0;
					for(auto j = d.fcloud->begin(); j!=d.fcloud->end();++j,++q){
						b[q].x = j->x*1000.f;
						b[q].y = j->y*1000.f;
						b[q].z = j->z*1000.f;
						b[q].r = j->r;
						b[q].g = j->g;
						b[q].b = j->b;
						b[q].a = 1.f;
					}
					d.buffer.data(POINT_ELEMCOUNT*d.fcloud->size(), (float*)b);
					d.va.bind();
					d.va.vertices(0, 3, d.buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_XYZ);
					d.va.vertices(1, 4, d.buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_RGBA);
					delete [] b;
				}

				d.deleted = false;
				d.cloud->clear();
				pcl::PCDReader pr;
				std::ostringstream oss;


				Log::info()<<"current cloud:"<<d.set<<" - "<<d.frame<<std::endl;
				oss<<"resources/rawclouds/"<<d.set<<"raw."<<d.frame<<".frame";
				bool ok = false;
				if(pr.read(oss.str(), *d.cloud) != 0){
					//Ran out of frames, save the current merged set.

					if(d.frame > 0){

						pcl::PCDWriter wrt;
						std::ostringstream fname;
						fname<<"resources/mergedclouds/"<<d.set<<".merge";
						wrt.write(fname.str(), *d.fcloud, true);

						if(d.autocollect){
							d.frame = 0;
							d.set++;
							std::ostringstream oss;
							oss<<"resources/rawclouds/"<<d.set<<"raw."<<d.frame<<".frame";
							pr = pcl::PCDReader();
							if(pr.read(oss.str(), *d.cloud) == 0){
								//This is a new set, clear the old set data.
					
								d.fcloud->clear();
								d.kpts->clear();
								d.frames.clear();

								ok = true;
					
							}
						}
					} else {
						//else out of frames and sets
						d.autocollect = false;
					}
				} else {
					ok = true;
				}

				if(d.cloud->size() == 0){
					d.frame+=3;
				}
				
				if(ok && d.cloud->size() > 0) {
					d.frame+=3;
					
					//Loaded frame.
					pcl::ApproximateVoxelGrid<pcl::PointXYZRGB> avg;
					float dst = 0.03;
					avg.setLeafSize(dst,dst,dst);
					avg.setDownsampleAllData(true);
					avg.setInputCloud(d.cloud);
					avg.filter(*d.cloud);

					Log::info()<<"Collected data"<<std::endl;
					normals(d.cloud, d.normalcloud);		
					Log::info()<<"Processed normals"<<std::endl;	
					tokeypoints(d.cloud, d.normals);
					Log::info()<<"Processed keypoints"<<std::endl;
					d.frames.push_back(framedata());
			
					if(d.frames.size() > 1){
						d.frames.back().trans = d.frames[d.frames.size()-2].trans;
					}

					toframe(d.normalcloud, d.normals, d.frames.back());

					pcl::transformPointCloudWithNormals(*d.frames.back().points,*d.frames.back().points, d.frames.back().trans);
					pcl::transformPointCloudWithNormals(*d.frames.back().keypoints,*d.frames.back().keypoints, d.frames.back().trans);

					d.frames.back().rebuild(d.vb, d.buf2);
					Log::info()<<"Built frame"<<std::endl;
				
			
					if(d.frames.size() > 1){
						auto lim = 0.3f;
				
						auto m = (Eigen::Matrix4f) Eigen::Matrix4f::Identity();

						pcl::IterativeClosestPoint<pcl::PointXYZRGBNormal, pcl::PointXYZRGBNormal> p;

						PRep prep;
						float alpha[4] = {1.0,1.0,1.0,1.0};
						prep.setRescaleValues(alpha);

						p.setPointRepresentation(boost::make_shared<const PRep>(prep));

						p.setRANSACIterations(20);
						p.setRANSACOutlierRejectionThreshold(0.1f);
						p.setMaximumIterations(30);
						p.setTransformationEpsilon(0.00001f);
			
						p.setInputCloud(d.frames.back().keypoints);
						p.setInputTarget(d.kpts);

						pcl::PointCloud<pcl::PointXYZRGBNormal> out;

						int successes = 0;

						for(int i=0;i<5;++i){
							p.setMaxCorrespondenceDistance(lim);
							p.align(out, m);
							if(abs(((p.getLastIncrementalTransformation()-m).sum()) < p.getTransformationEpsilon())){
								lim = lim*0.9;
							}	
							m=p.getFinalTransformation();
							successes++;
						}
						Log::info()<<"icp done"<<std::endl;
						Log::info()<<p.getFinalTransformation()<<std::endl;

						//apply transform
						if(successes > 3){
							pcl::transformPointCloudWithNormals(*d.frames.back().points, *d.frames.back().points, m);
							pcl::transformPointCloudWithNormals(*d.frames.back().keypoints, *d.frames.back().keypoints, m);
							d.frames.back().trans *= m;
							d.frames.back().rebuild(d.vb, d.buf2);
						}
					}
				}
			
			
				d.mode++;
				if(d.autocollect) d.mode = 0;

			}
		

			glClearColor(0.0,0.0,0.0,1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			float speed = 0.1;
		
			d.p.use();
			glm::mat4 cam = 
					glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 500000.0f)
					*glm::mat4_cast(glm::rotate(glm::quat(), -d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.c.rot.x, glm::vec3(0,1,0)))
					*glm::translate(glm::mat4(1.0f), d.c.trans*speed*glm::vec3(-1.f,-1.f,1.f));

			speed = 1.5;
		
			d.p.set("m", cam);
			d.vb.draw(0, d.frames.back().points->size(), 1, GL_POINTS);
			d.va.draw(0, d.fcloud->size(), 1, GL_POINTS);
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