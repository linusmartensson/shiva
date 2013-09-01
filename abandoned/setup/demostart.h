#pragma once
#include"stdafx.h"
#include"controllers.h"
#include"data.h"
#include"state.h"
#include"texture.h"
#include"state.h"
#include"postprocessor.h"

//Bar calculation helper variable. 60sec/bpm
const float beatdist = 1000.f*60/148.f;

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

class demostart : public shiva::state {

	struct exit : public shiva::state{
		bool stateUpdate(){
			if(!shiva::core::pause())
				shiva::core::stop();
			return true;
		}
	};

	int timestep;

private:
	shiva::controls::tcontrol<state> *scenes;
	template<typename T>
	void add(float start, float end){
		scenes->add(new offstate(new T(this), start, end), start, end);
	}
public:
	demostart(int argc, char **argv) : timestep(1) {
		glEnable(GL_MULTISAMPLE);
		scenes = new shiva::controls::tcontrol<state>;		
		
		hdrstate *hdr;
		{
			hdrpipe *hdrp;
			sl.add(&(hdrp = new hdrpipe)->add(scenes));
			hdr = hdrp;
		}
		scenes->add(new particular, t(0), t(999999));
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
		sl.kinput(k,s);
	}

};
