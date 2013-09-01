#include"state.h"
#include"shaders.h"
#include"clengine.h"
#include"resourcetracker.h"
#include"music.h"
#include<algorithm>
#include<fstream>

struct cpos{
	glm::vec3 trans;
	glm::vec2 rot;
	float time;

	cpos operator+(const cpos &rhs) const {
		cpos c;
		c.trans = trans+rhs.trans;
		c.rot = rot+rhs.rot;
		return c;
	}
	cpos operator*(const float f) const {
		cpos c = *this;
		c.trans *= f;
		c.rot *= f;
		return c;
	}
	cpos operator-(const cpos &rhs) const {
		return *this+rhs*-1.f;
	}
	cpos operator/(const float f) const {
		return *this*(1.f/f);
	}

	cpos() : trans(0.f), rot(0.f), time(0.f) {}

	cpos(glm::vec3 trans, glm::vec3 rot) : trans(trans), rot(rot), time(0.f) {}
	virtual ~cpos(){}
};

std::istream& operator>>(std::istream &iss, cpos &v){
	iss>>v.trans.x;
	iss>>v.trans.y;
	iss>>v.trans.z;
	iss>>v.rot.x;
	iss>>v.rot.y;
	iss>>v.time;
	return iss;
}
std::ostream& operator<<(std::ostream &oss, const cpos &v){
	oss<<v.trans.x<<" ";
	oss<<v.trans.y<<" ";
	oss<<v.trans.z<<" ";
	oss<<v.rot.x<<" ";
	oss<<v.rot.y<<" ";
	oss<<v.time<<"\n";
	return oss;
}

cpos hermiteInterpolate(
	cpos y0, cpos y1,
	cpos y2, cpos y3,
	float mu,
	float tension,
	float bias
	){
	cpos m0,m1;
	float mu2,mu3;
	float a0, a1, a2, a3;
	mu2 = mu*mu;
	mu3 = mu2*mu;
	tension = (1.f-tension)/2.f;
	m0 = (y1-y0)*(1.f+bias)*tension;
	m0 = m0+(y2-y1)*(1.f-bias)*tension;
	m1 = (y2-y1)*(1.f+bias)*tension;
	m1 = m1+(y3-y2)*(1.f-bias)*tension;
	a0 = mu3*2.f - 3.f*mu2 + 1.f;
	a1 = mu3 - 2.f*mu2 + mu;
	a2 = mu3 - mu2;
	a3 = mu3*-2.f + mu2*3.f;

	return y1*a0 + m0*a1 + m1*a2 + y2*a3;
}

cpos lerp(cpos y1, cpos y2, float m){
	return y1*(1.f-m)+y2*m;
}

struct camera : public cpos {
	
	glm::vec3 prevtrans;
	std::vector<cpos> c;
	bool automate;
	std::string name;
	glm::vec2 prevf;
	bool left,right,up,down;

	camera() : automate(true), left(false), right(false), up(false), down(false) {
		prevf.x = shiva::core::width()/2.f;
			prevf.y = shiva::core::height()/2.f;
	}

	void run(shiva::eventmap &r, float t, bool paused){
		time = t;
		prevtrans = trans;
		if(r.count("kinput")){

			if(shiva::core::down('Q')){
				automate = !automate;
			}
			
			if(automate) return;
			
			if(shiva::core::down('E')){
				size_t i=0;
				/*for(;i<c.size() && c[i].time<=time;++i); 
				c.insert(c.begin()+i, cpos(*this));*/
				Log::info()<<cpos(*this);
			}
			if(shiva::core::down('L')){
				rebuild();
			}
			if(shiva::core::down('1')){
				if(name != ""){
					std::ofstream off(name);
					for(size_t i=0;i<c.size();++i){
						off<<c[i];
					}
				}
			}

			if(shiva::core::down('W')){
				up = true;
			} else {
				up = false;
			}
			if(shiva::core::down('S')){
				down = true;
			} else {
				down = false;
			}
			if(shiva::core::down('A')){
				left = true;
			} else left = false;

			if(shiva::core::down('D')){
				right = true;
			} else right = false;
			
		}
		if(r.count("minput")){
			std::pair<int,int> f = boost::any_cast<std::pair<int,int>>(r["minput"]);

			

			if(automate) return;
			rot.x += (f.first-prevf.x)*0.1f;
			rot.y += (f.second-prevf.y)*0.1f;

			prevf = glm::vec2(f.first,f.second);
		}

		if(automate){
			cpos s = *this, d = *this, ss = *this, dd = *this;
			size_t i=0;
			for(;i<=c.size();++i){
				if(i<c.size() && c[i].time < time) continue;
				
				if(i-2 >= 0 && c.size()>0) ss = c[i-2];
				if(i-1 >= 0 && c.size()>0) s = c[i-1];
				if(i < c.size()) d = c[i];
				if(i+1 < c.size()) dd = c[i+1];

				break;
			}
			if(i < c.size()) {
				float dds = d.time-s.time;
				float i = ((d.time-time)/dds);

				trans = s.trans*i+d.trans*(1.f-i);
				rot = s.rot*i+d.rot*(1.f-i);

				cpos c = hermiteInterpolate(ss,s,d,dd, 1.f-i,0.f,0.f);
				trans = c.trans;
				rot = c.rot;
			}

		} else {
			
			trans = trans + 
			(glm::vec4(((left?1.0:0.0)-(right?1.0:0.0))*100.f,0.f,((up?1.0:0.0)-(down?1.0:0.0))*100.f,1.0f)*
			glm::mat4_cast(
				glm::rotate(glm::quat(), rot.y, glm::vec3(1,0,0))*
				glm::rotate(glm::quat(), rot.x, glm::vec3(0,1,0)))
			).swizzle(glm::X, glm::Y, glm::Z);
		}
			
		/*rot.y = 30.0;
		float de = (float)time*0.4f;
		rot.x = de*100*9.f/16.f-70;
		trans.z = -5000*sin(de);
		trans.x = -5000*cos(de);
		trans.y = -300.f;*/
	}
	void rebuild(){
		c.clear();
		trans=glm::vec3(0.0);
		rot=glm::vec2(0.0);
		if(name!=""){
			this->name = name;
			std::ifstream iff(name);
			while(iff.good()){
				cpos cc;
				iff>>cc;
				if(iff.bad()||iff.eof()||iff.fail())break;
				c.push_back(cc);
			}
		}
		if(c.size() == 0){
			c.push_back(cpos(*this));
			c.push_back(cpos(*this));
		}
	}
	void init(shiva::state *s, std::string name=""){
		
		s->reg("kinput");
		s->reg("minput");

		if(name != ""){
			name = "resources/"+name;
		}
		this->name = name;
		rebuild();
	}
};



struct startdata{
	int pointcount;
	FSShader fss;
	Program p1,p2;
	std::vector<CLProgram*> cl;
	std::vector<std::string> clp;
	float aspectRatio;
	Buffer<GLfloat> pos;
	Buffer<GLfloat> acc;
	Buffer<GLfloat> state;
	Buffer<GLfloat> color;
	Buffer<GLfloat> screenposition;
	Buffer<GLfloat> oldscreenposition;
	Buffer<GLfloat> pcam, pcam2, pcam3;
	std::vector<float> times;
	uint timespos;
	VArray va;
	bool reload;
	bool paused;
	float time;
	float startTime;
	float prevTime;
	camera c;
	music *m;

	void rebuild(){
		for(size_t i=0;i<cl.size();++i){
			cl[i]->rebuild(loadFile(clp[i], true));
		}
		  p1.rebuild("statedata/dd/minimal1.frag","statedata/dd/minimal1.geom","statedata/dd/minimal1.vert")
		, p2.rebuild("statedata/dd/minimal2.frag","statedata/dd/minimal2.geom","statedata/dd/minimal2.vert");
		  c.rebuild();
		  m->setTime(0);
	}

	startdata() : reload(false), /*trans(0.0,-300.0,0.0), xd(0.0), yd(0.0),*/ pointcount(200000), fss("statedata/dd/output.frag"), timespos(0)
	,p1("statedata/dd/minimal1.frag","statedata/dd/minimal1.geom","statedata/dd/minimal1.vert")
	,p2("statedata/dd/minimal2.frag","statedata/dd/minimal2.geom","statedata/dd/minimal2.vert")
	,paused(false), m(new music("statedata/dd/french_sexy_maid.mp3")) {
		clp.push_back("builtins/tools.cl statedata/dd/t0.cl statedata/dd/particleengine.cl");
		clp.push_back("builtins/tools.cl statedata/dd/t1.cl statedata/dd/particleengine.cl");
		clp.push_back("builtins/tools.cl statedata/dd/t2.cl statedata/dd/particleengine.cl");
		clp.push_back("builtins/tools.cl statedata/dd/t3.cl statedata/dd/particleengine.cl");
		clp.push_back("builtins/tools.cl statedata/dd/t4.cl statedata/dd/particleengine.cl");
		for(size_t i=0;i<clp.size();++i){
			cl.push_back(new CLProgram(loadFile(clp[i])));
		}
	}
	void destroy(){
		for(auto i=cl.begin();i!=cl.end();++i){
			delete *i;
		}
		delete m;
	}
};

class particles : public shiva::behaviour {
	virtual void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		startdata &d = (boost::any_cast<startdata&>(data));
		bool move = false;
		if(received.count("resourcechange")){
			d.reload = true;
		}
		if(received.count("kinput")){
			std::pair<int,bool> g = boost::any_cast<std::pair<int,bool>>(received["kinput"]);
			if(g.first == GLFW_KEY_ESC)
				shiva::core::stop();
			if(g.first == GLFW_KEY_RIGHT){
				d.startTime -= 1.0;
				move = true;
			}
			if(g.first == GLFW_KEY_LEFT){
				d.startTime += 1.0;
				move = true;
			}
			if(g.first == GLFW_KEY_SPACE && g.second == true){
				d.paused = !d.paused;
				if(d.paused == false){
					d.startTime += (float)glfwGetTime()-d.prevTime;
					d.m->pause(false);
				} else {
					d.prevTime = (float)glfwGetTime();
					d.m->pause(true);
				}
			}
		}
		if(!d.paused){
			d.time = (float)glfwGetTime()-d.startTime;
		} else {
			d.time = d.prevTime-d.startTime;
			move = true;
		}
		if(move)
			d.m->setTime(d.time);
		if(d.time < 0){
			d.startTime += d.time;
		}
		
		d.aspectRatio = 16.f/9.f;
		
		d.c.run(received, d.time, d.paused);
	}


	virtual void render(shiva::childvec children, boost::any &data) const {
		
		startdata &t = (boost::any_cast<startdata&>(data));

		if(t.reload){
			reload(data);
		}


		startdata &d = (boost::any_cast<startdata&>(data));

		if(d.times[d.timespos] < d.time) 
			d.timespos++;
		
		if(d.timespos>0 && d.times[d.timespos-1] > d.time) 
			d.timespos--;

		if(d.timespos >= d.times.size()) shiva::core::stop();
		float toff = d.timespos>0?(d.time - d.times[d.timespos-1])*1000.0f:d.time*1000.0f;
		int active = d.timespos;//int(d.time/10)%4;

		

		glDisable(GL_BLEND);
		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDisable(GL_DEPTH_TEST);
		
		glEnable(GL_BLEND);
		
		float speed = 1.0;

		glm::mat4 cam = glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), d.c.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.c.trans*speed));
		

		d.pcam.data(16, glm::value_ptr(cam));

		glm::mat4 rotcam = (glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)*
			glm::mat4_cast(glm::rotate(glm::quat(), d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), d.c.rot.x, glm::vec3(0,1,0))));
		
		glm::vec4 ptrans(d.c.trans*speed, 0.0);
		d.pcam2.data(16, glm::value_ptr(rotcam));
		d.pcam3.data(4, glm::value_ptr(ptrans));
		for(size_t i=0;i<d.cl.size();++i){
			d.cl[i]->set("particleengine", 9, d.paused?1.f:0.f);
			d.cl[i]->set("particleengine", 5, toff);
			d.cl[i]->set("particleengine", 6, d.pcam);
			d.cl[i]->set("particleengine", 10, d.pcam2);
			d.cl[i]->set("particleengine", 11, d.pcam3);
		}
		d.p1.bindAttrib(0, "position");
		d.p2.bindAttrib(0, "position");
		d.p1.bindAttrib(1, "position2");
		d.p2.bindAttrib(1, "position2");
		d.p1.bindAttrib(2, "color");
		d.p2.bindAttrib(2, "color");
		
		switch(active){
			case 0:
				glBlendFunc(GL_ONE,GL_ONE);
				d.cl[0]->run("particleengine", d.pointcount);
				d.p1.use();
				d.va.draw(0,d.pointcount,1,GL_POINTS);
			break;
			case 1:
				glBlendFunc(GL_ONE,GL_ONE);
				d.cl[1]->run("particleengine", d.pointcount);
				d.p1.use();
				d.va.draw(0,d.pointcount,1,GL_POINTS);
			break;
			case 2:
				glBlendFunc(GL_ONE,GL_ONE);
				d.cl[0]->run("particleengine", d.pointcount);
				d.p1.use();
				d.va.draw(0,d.pointcount,1,GL_POINTS);
			break;
			case 3:
				d.cl[2]->run("particleengine", d.pointcount);
				d.p1.use();
				glBlendFunc(GL_ONE,GL_ONE);
				d.va.draw(d.pointcount/2,d.pointcount/2,1,GL_POINTS);
				d.p2.use();
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
				d.va.draw(0,d.pointcount/2,1,GL_POINTS);
			break;
			case 4:
				d.cl[3]->run("particleengine", d.pointcount/4);
				d.p1.use();
				glBlendFunc(GL_ONE,GL_ONE);
				d.va.draw(d.pointcount/8,d.pointcount/8,1,GL_POINTS);
				d.p2.use();
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
				d.va.draw(0,d.pointcount/8,1,GL_POINTS);
			break;
			case 5:
				d.cl[4]->run("particleengine", d.pointcount/4);
				d.p1.use();
				glBlendFunc(GL_ONE,GL_ONE);
				d.va.draw(d.pointcount/8,d.pointcount/8,1,GL_POINTS);
				d.p2.use();
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
				d.va.draw(0,d.pointcount/8,1,GL_POINTS);
			break;
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ONE);
		d.fss.clear(false);
		d.fss.set("time", d.time);
		//d.fss.display();
	}
	void reload(boost::any &data) const {
		auto &d = boost::any_cast<startdata&>(data);
		
		d.rebuild();
		d.timespos = 0;
		d.times.clear();
		d.times.push_back(10);
		d.times.push_back(23.2);
		d.times.push_back(30);
		d.times.push_back(69.3);
		d.times.push_back(75);
		d.times.push_back(113);
		
		d.startTime = (float)glfwGetTime();
		d.time = (float)glfwGetTime()-d.startTime;
		d.aspectRatio = 16.f/9.f;
		
		/*d.yd = 30.0;*/
		float de = (float)d.time*0.4f;
		/*d.xd = de*100*9.f/16.f-70;
		d.prevtrans = d.trans;
		d.trans.z = -5000*sin(de);
		d.trans.x = -5000*cos(de);*/

		std::vector<GLfloat> glf;
		for(int i=0;i<4*d.pointcount;++i){
			glf.push_back((rand()%65536)/65536.f-0.5f);
		}
		d.pos.data(glf);
		d.acc.data(glf);
		d.state.data(glf);
		d.color.data(glf);
		d.screenposition.data(glf);
		d.oldscreenposition.data(glf);
		

		for(size_t i=0;i<d.cl.size();++i){
			d.cl[i]->genkernel("particleengine");
			d.cl[i]->set("particleengine", 0, d.pos);
			d.cl[i]->set("particleengine", 1, d.acc);
			d.cl[i]->set("particleengine", 2, d.state);
			d.cl[i]->set("particleengine", 3, d.color);
			d.cl[i]->set("particleengine", 4, 0.01f);
			d.cl[i]->set("particleengine", 7, d.screenposition);
			d.cl[i]->set("particleengine", 8, d.oldscreenposition);
			d.cl[i]->set("particleengine", 9, 0.f);
		}

		d.va.vertices(0,4, d.screenposition);
		d.va.vertices(1,4, d.oldscreenposition);
		d.va.vertices(2,4, d.color);	
		
		d.reload = false;
		if(d.paused == false) d.m->pause(false);
	}
	virtual void init(shiva::state *instance, boost::any &data) const {
		instance->reg("kinput");
		instance->reg("resourcechange");
		data = boost::any(startdata());
		
		reload(data);
		
		auto &d = boost::any_cast<startdata&>(data);
		d.c.init(instance, "statedata/dd/dd.camera");
		
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {
		
		auto &d = boost::any_cast<startdata&>(data);
		d.destroy();
	}
};
behaviour_add(particles);