#pragma once
#include"stdafx.h"
#include"controllers.h"
#include"data.h"
#include"state.h"
#include"texture.h"
#include"state.h"
#include"postprocessor.h"
#include"state/common/offstate.h"

//Bar calculation helper variable. 60sec/bpm
const float beatdist = 1000.f*60/140.f;

//Sound help functions

//Convert time to bars, c=1.0 is start of song to comply with most audio software.
float t(float c){
	return (c-1.f)*beatdist*4.f;
}

//Time since last beat for physics thread
float tslb(){
	return ::fmodf(shiva::core::physicsTime(), beatdist);
}

//Time until next beat for physics thread
float tslbi(){
	return beatdist - tslb();
}

//Time since last beat for render thread
float tslbr(){
	return ::fmodf(shiva::core::renderTime(), beatdist);
}

//Time until next beat for render thread
float tslbir(){
	return beatdist - tslbr();
}

#include"state/pipeline/hdr.h"
#include"state/pipeline/particular.h"
#include"state/pipeline/credits.h"

class demostart : public shiva::state {

	struct exit : public shiva::state{
		bool stateUpdate(){
			if(!shiva::core::pause())
				shiva::core::stop();
			return true;
		}
	};

	int timestep;
	bool inputAllowed;
private:
	shiva::controls::tcontrol<state> *scenes;
	template<typename T>
	void add(float start, float end){
		scenes->add(new T(), start, end);
	}

	glm::vec3 trans;
	float xd,yd;
	static const unsigned int particleCount = 10000;
		VArray vb;
		Buffer<GLfloat> 
			screenposition, 
			oldscreenposition, 
			position, 
			color, 
			acceleration, 
			state, 
			rcolor,
			rscreenposition, 
			roldscreenposition;
	void add(std::string clp, std::string frag, std::string geom, std::string vert, float start, float end){
		scenes->add(
			new particular(
				particleCount, vb, screenposition, oldscreenposition, position, 
				color, acceleration, state, 
				rcolor, rscreenposition, roldscreenposition,
				std::string("statedata/pipeline/particular/")+clp,
				trans,
				xd,
				yd,
				std::string("statedata/pipeline/particular/")+frag,
				std::string("statedata/pipeline/particular/")+geom,
				std::string("statedata/pipeline/particular/")+vert), 
			t(start), 
			t(end));
	}
public:
	demostart(int argc, char **argv) : timestep(1),
			screenposition(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
			oldscreenposition(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
			position(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
			color(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
			acceleration(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
			state(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
			rcolor(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY),
			rscreenposition(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
			roldscreenposition(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY) {
		glEnable(GL_MULTISAMPLE);
		scenes = new shiva::controls::tcontrol<shiva::state>;		
		
		inputAllowed = true;

		hdrstate *hdr;
		{
			hdrpipe *hdrp;
			sl.add(&(hdrp = new hdrpipe)->add(scenes));
			hdr = hdrp;
		}
	    
		std::vector<GLfloat> tmp;
		
		vb.vertices(0,4, rscreenposition);
		vb.vertices(1,4, roldscreenposition);
		vb.vertices(2,4, rcolor);	
		VArray::unbind();
		
		trans = glm::vec3(0.0,0.0,-10.0);
		xd = 0.0f;
		yd = 0.0f;
		camera *c = new camera("pipeline", trans, xd, yd, 'W', 'S', 'A', 'D');
		sl.add(c);

		for(unsigned int i=0;i<particleCount*4;++i){
			tmp.push_back(0.0f);
		}
		screenposition.data(tmp);
		oldscreenposition.data(tmp);
		position.data(tmp);
		acceleration.data(tmp);
		state.data(tmp);
		color.data(tmp);
		
		//add("twist.cl", "minimallines.frag", "minimallines.geom", "minimal.vert", 0, 61);
		
		add("torus.cl", "minimal.frag", "minimal.geom", "minimal.vert", 1, 33);
		add("twist.cl", "minimallines.frag", "minimallines.geom", "minimal.vert", 33, 61);
		add("pillarworld.cl", "minimal.frag", "minimal.geom","minimal.vert", 61, 91);
		
		add<credits>(t(81), t(9999));
		add<exit>(t(90), t(9999));
	}

	
	virtual void winput(bool up){
		if(inputAllowed) sl.winput(up);
	}

	virtual void binput(int btn, bool state){
		if(inputAllowed) sl.binput(btn, state);
	}

	virtual void minput(int x, int y){
		if(inputAllowed) sl.minput(x, y);
	}

	void kinput(int k, bool s){
		if(s && k == GLFW_KEY_SPACE) shiva::core::flipPause();
		if(s && k == GLFW_KEY_ESC) shiva::core::stop();
		if(shiva::core::pause()){
			if(s && k == GLFW_KEY_PAGEUP) timestep++;
			if(s && k == GLFW_KEY_PAGEDOWN) timestep--;
			if(s && k == GLFW_KEY_LEFT) shiva::core::dectime(DWORD(timestep>=0?t(2)*timestep:t(2)/(-timestep)));
			if(s && k == GLFW_KEY_RIGHT) shiva::core::inctime(DWORD(timestep>=0?t(2)*timestep:t(2)/(-timestep)));
		}
		if(inputAllowed) sl.kinput(k,s);
	}
	
	bool stateUpdate(){
		rscreenposition.cloneContent(screenposition);
		roldscreenposition.cloneContent(oldscreenposition);
		rcolor.cloneContent(color);
		sl.stateUpdate();
		return true;
	}
};
