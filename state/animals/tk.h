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

#define POINT_ELEMCOUNT 8
#define POINT_XYZ		 0
#define POINT_RGBA		 4

struct action{
	float prev;
	int curr;
	bool changed;
private:
	std::vector<std::pair<float, int>> set;
	std::string src;
public:

	operator int(){
		return curr;
	}

	action(std::string src) : curr(0), changed(true), src(src) {
		load();
	}

	bool update(float time){
		auto i = set.begin();
		for(;i!=set.end()&&i->first<prev;++i);
		int j=0;
		for(;i!=set.end()&&i->first<time;++i, ++j) curr = i->second; 
		prev=time;
		return changed=j>0;
	}

	void change(float time, int value){
		auto i = set.begin();
		auto p = set.begin();
		for(;i!=set.end()&&i->first<time;++i) if(i!=set.begin()) p++;
		p->second = value;
		curr = value;
		prev = time;
		changed = true;
	}

	void add(float time, int value){
		auto i = set.begin();
		for(;i!=set.end()&&i->first<time;++i);
		set.insert(i, std::make_pair(time, value));
		curr = value;
		prev = time;
		changed = true;
	}

	void save(){
		std::ofstream o(src.c_str());
		o<<set.size()<<" ";
		for(auto i=set.begin(); i!=set.end(); ++i){
			o<<i->first<<" "<<i->second<<" ";
		}
	}
	void load(){
		std::ifstream o(src.c_str());
		int i=0;
		o>>i;
		for(int j=0;j<i;++j){
			float a; int b;
			o>>a>>b;
			//inefficient, but careface.
			add(a, b);
		}
		if(set.size() == 0) add(0,0);
		prev = 0;
		curr = 0;
		changed = true;
	}
};

struct point{
	float x,y,z,w;
	float r,g,b,a;
};
struct framedata {
	point *b;
	int count;
	void rebuild(VArray &va, Buffer<GLfloat> &buffer){
		buffer.data(POINT_ELEMCOUNT*count, (float*)b);
		va.bind();
		va.vertices(0, 3, buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_XYZ);
		va.vertices(1, 4, buffer,0,sizeof(GLfloat)*POINT_ELEMCOUNT,POINT_RGBA);
	}
};
struct tgdata{
	bool reload;
	camera c;
	music *m;
	action set, clon;
	FrameBuffer fb;
	PNGImage gz;
	Texture color, depth, greet;
	FSShader fss;
	std::vector<framedata> frames;

	

	CLProgram clp;
	Buffer<GLfloat> buffer;
	
	VArray va;
	Program p; 

	tgdata() : reload(false), m(new music("statedata/animals/animals.mp3")), set("resources/statedata/dq/set.act"), clon("resources/statedata/dq/clon.act"),
		color(8, GL_RGBA16F), depth(8, GL_DEPTH_COMPONENT32F), fss("statedata/dq/hdr.frag"),
		p(	"statedata/df/simple.frag", 
			"statedata/df/quad.geom", 
			"statedata/df/pt.vert"),
		clp(loadFile("builtins/tools.cl statedata/dq/clp.cl")),
		gz("statedata/dq/greetz.png"),
		greet(1, GL_RGBA8) {}
	~tgdata(){}
	void refresh(){
		reload = false;

		p.rebuild("statedata/df/simple.frag", "statedata/df/quad.geom", "statedata/df/pt.vert");

		p.bindAttrib(0, "pos");
		p.bindAttrib(1, "color");
		
		fss.rebuild("statedata/dq/hdr.frag");

		clp.rebuild(loadFile("builtins/tools.cl statedata/dq/clp.cl"));
		clp.genkernel("engine");

		set.load();
		clon.load();

		greet.bind();
		glTexImage2D(GL_TEXTURE_2D_MULTISAMPLE, 0, GL_RGBA8, gz.width(), gz.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, gz.data());

		m->time(0);
	}
};

class tk : public shiva::behaviour {

	//IO handling
	virtual void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		auto &d = boost::any_cast<tgdata&>(data);
		d.set.update(d.m->time());
		d.clon.update(d.m->time());
		if(received.count("kinput")){
			std::pair<int,bool> g = boost::any_cast<std::pair<int,bool>>(received["kinput"]);
			if(g.first == GLFW_KEY_ESC)
				shiva::core::stop();
			if(g.first == GLFW_KEY_UP && g.second==true){
				d.set.add(d.m->time(), d.set.curr+1);
			}
			if(g.first == GLFW_KEY_DOWN && g.second==true){
				d.set.add(d.m->time(), d.set.curr-1);
			}
			if(g.first == GLFW_KEY_SPACE && g.second==true){
				d.m->pause(!d.m->pause());
			}
			if(g.first == GLFW_KEY_LEFT && g.second == true){
				d.m->time(d.m->time()-1);
			}
			if(g.first == GLFW_KEY_RIGHT && g.second == true){
				d.m->time(d.m->time()+1);
			}
			if(g.first == 'K' && g.second == true){
				d.clon.add(d.m->time(), d.clon.curr+1);
			}
			if(g.first == 'M' && g.second == true){
				d.clon.add(d.m->time(), d.clon.curr-1);
			}
			if(g.first == 'O' && g.second == true){
				d.clon.save();
				d.set.save();
			}
		}		
		if(received.count("resourcechange")){
			d.reload = true;
		}
		
		d.set.curr %= d.frames.size();
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
		
			if(d.set.changed){
				d.frames[d.set.curr].rebuild(d.va, d.buffer);
				d.clp.erase(d.buffer);
				d.clp.set("engine", 0, d.buffer);
			}
			
			if(!d.m->pause() && d.clon.curr != 0){
				d.clp.set("engine", 1, (float)d.m->pause());
				d.clp.set("engine", 2, (float)d.clon.curr);
				d.clp.run("engine", /*sz*/);
			}

			glClearColor(0.0,0.0,0.0,1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			float speed = 0.1;
		
			d.p.use();
			glm::mat4 cam = 
					glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 500000.0f)
					*glm::mat4_cast(glm::rotate(glm::quat(), -d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.c.rot.x, glm::vec3(0,1,0)))
					*glm::translate(glm::mat4(1.0f), d.c.trans*speed*glm::vec3(-1.f,-1.f,1.f));
			d.p.set("m", cam);
			//d.va.draw(0, d.frames[d.set.curr].count, 1, GL_POINTS);
		});
		d.fss.program().use();
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

		d.c.init(instance, "statedata/animals/0.camera");
		d.refresh();
		d.m->pause(false);
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(tk);