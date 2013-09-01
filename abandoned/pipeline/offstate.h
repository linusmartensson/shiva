#pragma once

#include"state.h"
#include"shiva.h"

class offstate : public shiva::state{
private:
	float start;
	float end;
		
public:
	offstate(shiva::state *s, float start, float end) : start(start), end(end) {
		sl.add(s);
	}

	virtual void winput(bool up){
		shiva::core::pushpoffset(DWORD(start));
		shiva::core::pushroffset(DWORD(start));
		sl.winput(up);
		shiva::core::poppoffset();
		shiva::core::poproffset();
	}
	virtual void binput(int btn, bool state){
		shiva::core::pushpoffset(DWORD(start));
		shiva::core::pushroffset(DWORD(start));
		sl.binput(btn,state);
		shiva::core::poppoffset();
		shiva::core::poproffset();
	}
	virtual void minput(int x, int y){
		shiva::core::pushpoffset(DWORD(start));
		shiva::core::pushroffset(DWORD(start));
		sl.minput(x,y);
		shiva::core::poppoffset();
		shiva::core::poproffset();
	}
	virtual void kinput(int key, bool state){
		shiva::core::pushpoffset(DWORD(start));
		shiva::core::pushroffset(DWORD(start));
		sl.kinput(key,state);
		shiva::core::poppoffset();
		shiva::core::poproffset();
	}
	virtual void display(){
		shiva::core::pushroffset(DWORD(start));
		sl.display();
		shiva::core::poproffset();
	}
	virtual void update(){
		shiva::core::pushpoffset(DWORD(start));
		sl.update();
		shiva::core::poppoffset();
	}
	virtual bool stateUpdate(){
		shiva::core::pushpoffset(DWORD(start));
		shiva::core::pushroffset(DWORD(start));
		return sl.stateUpdate();
		shiva::core::poppoffset();
		shiva::core::poproffset();
	}
	virtual void reshape(int w, int h){
		shiva::core::pushpoffset(DWORD(start));
		shiva::core::pushroffset(DWORD(start));
		sl.reshape(w,h);
		shiva::core::poppoffset();
		shiva::core::poproffset();
	}
		
};