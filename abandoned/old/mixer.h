#pragma once

#include"stdafx.h"
#include"state.h"
#include"controllables.h"


class mixer : public shiva::state {
	std::string mixername;
	struct controlset{
		int pos, neg, s,n,p;
		controlset(int pos, int neg, int s, int n, int p) : pos(pos), neg(neg), s(s), n(n), p(p) {}
	};
	std::vector<controlset> controlsets;
public:
	mixer(std::string mixername="") : mixername(mixername) {
		controlsets.push_back(controlset('+','-','0',GLFW_KEY_F10,'P'));
		controlsets.push_back(controlset('Q','A','1','Z',GLFW_KEY_F1));
		controlsets.push_back(controlset('W','S','2','X',GLFW_KEY_F2));
		controlsets.push_back(controlset('E','D','3','C',GLFW_KEY_F3));
		controlsets.push_back(controlset('R','F','4','V',GLFW_KEY_F4));
		controlsets.push_back(controlset('T','G','5','B',GLFW_KEY_F5));
		controlsets.push_back(controlset('Y','H','6','N',GLFW_KEY_F6));
		controlsets.push_back(controlset('U','J','7','M',GLFW_KEY_F7));
		controlsets.push_back(controlset('I','K','8',',',GLFW_KEY_F8));
		controlsets.push_back(controlset('O','L','9','.',GLFW_KEY_F9));
	}
	
	template<typename T>
	void add(unsigned int slot, std::string name, T &target, std::function<T(T,int)> f, bool continuous=false){
		
		std::ostringstream oss;
		oss<<"interpolations/_builtin_mixer_"<<mixername<<"_"<<name<<"_"<<slot<<".ip";
		
		controlset c = controlsets[slot];

		shiva::controllable::make_control(
			sl, 
			oss.str(), 
			target, 
			f, 
			shiva::controllable::key, 
			c.pos, 
			c.neg, 
			continuous,
			c.s, 
			GLFW_KEY_F5, 
			c.n, 
			c.p);
	}
};