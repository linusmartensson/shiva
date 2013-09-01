
#pragma once
#include"state.h"
#include"shaders.h"
#include"clengine.h"
#include"resourcetracker.h"
#include"framebuffer.h"
#include"music.h"
#include<algorithm>
#include<fstream>
#include"fonts.h"

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
cpos hermiteInterpolate(cpos y0, cpos y1,cpos y2, cpos y3,float mu,float tension,float bias) {
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
	bool automate, norot;
	std::string name;
	glm::vec2 prevf;
	bool left,right,up,down;

	camera(bool norot=false) : automate(true), left(false), right(false), up(false), down(false), norot(norot) {
		prevf.x = shiva::core::width()/2.f;
		prevf.y = shiva::core::height()/2.f;
		
		c.clear();
		trans=glm::vec3(0.0);
		rot=glm::vec2(0.0);
	}

	void save(){
		if(name != ""){
			std::ofstream off(name);
			for(size_t i=0;i<c.size();++i){
				off<<c[i];
			}
		}
	}
	std::ostream& save(std::ostream &off){
		off<<c.size()<<std::endl;
		for(size_t i=0;i<c.size();++i){
			off<<c[i];
		}
		return off;
	}
	std::istream& rebuild(std::istream &iff){
		c.clear();
		trans=glm::vec3(0.0);
		rot=glm::vec2(0.0);
		automate = false;
		this->name = name;
		size_t s=0;
		iff>>s;
		for(size_t i=0;i<s;++i){
			cpos cc;
			iff>>cc;
			c.push_back(cc);
		}
		if(c.size() == 0){
			c.push_back(cpos(*this));
			c.push_back(cpos(*this));
		}
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
			rot.x -= (f.first-prevf.x)*0.1f;
			rot.y -= (f.second-prevf.y)*0.1f;

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
			
			if(!norot){
			trans = trans + 
			(glm::vec4(((left?1.0:0.0)-(right?1.0:0.0))*-100.f,0.f,((up?1.0:0.0)-(down?1.0:0.0))*100.f,1.0f)*
			glm::mat4_cast(
				glm::rotate(glm::quat(), rot.y, glm::vec3(1,0,0))*
				glm::rotate(glm::quat(), rot.x, glm::vec3(0,1,0)))
			).swizzle(glm::X, glm::Y, glm::Z);
			} else {
				trans = trans + 
				(glm::vec4(((left?1.0:0.0)-(right?1.0:0.0))*-100.f,0.f,((up?1.0:0.0)-(down?1.0:0.0))*100.f,1.0f)).swizzle(glm::X,glm::Y,glm::Z);
			}
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
		automate = false;
		if(name!=""){
			this->name = name;
			std::ifstream iff(name);
			while(iff.good()){
				cpos cc;
				iff>>cc;
				if(iff.bad()||iff.eof()||iff.fail())break;
				c.push_back(cc);
				automate = true;
			}
		}
		if(c.size() == 0){
			c.push_back(cpos(*this));
			c.push_back(cpos(*this));
		}
	}
	void init(shiva::state *s, std::string name=""){
		
		if(s!=0){
			s->reg("kinput");
			s->reg("minput");
		}
		if(name != ""){
			name = "resources/"+name;
		}
		this->name = name;
		rebuild();
	}
	std::istream& init(std::istream &iss){
		return rebuild(iss);
	}
};