#pragma once

#include<gltext.hpp>
#include"buffers.h"
#include"shaders.h"
#include"varray.h"
#include"log.h"

//Specialization of gltext which resets current shiva state & initiates cache to work around a bug.
class font : public gltext::Font{
	void reset(){
		BufferBase::reset();
		Program::reset();
		VArray::reset();
	}
	size_t w;
	font& operator=(const font &rhs){}
public:
	void cacheCharacters(const std::string &s){
		gltext::Font::cacheCharacters(s);
		reset();
	}
	void draw(const std::string &s){
		gltext::Font::draw(s);
		reset();
	}
	font(std::string s, int sz) : gltext::Font(s, sz){
//		cacheCharacters("1234567890+½!\"#¤%&/()=?`´§@£$€{[]}\\¨'^~*,.-;:_<>|ABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖabcdefghijklmnopqrstuvwxyzåäö");
		cacheCharacters("1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz&:-");
		w = gltext::Font::width('W');
	}
	
	size_t width(){
		return w;
	}
};