#pragma once
#include"stdafx.h"
#include"shiva.h"

class Texture{
	static uint genBufferId(){
		uint buf = 0;
		glGenTextures(1, &buf);
		return buf;
	}
	static uint genHeight(uint h){
		if(h<=0){h = shiva::core::height();}
		return h;
	}
	static uint genWidth(uint h){
		if(h<=0){h = shiva::core::width();}
		return h;
	}

	uint *refs;
public:
	const uint buf;
	const uint samples;
	const GLenum internalformat;
	const GLenum texturedims;
	const uint width;
	const uint height;
	void bind(){
		Log::GLErrors("before glBindTexture(texturedims, buf);");
		glBindTexture(texturedims, buf);
		Log::GLErrors("glBindTexture(texturedims, buf);");
	}
	Texture(uint samples, GLenum internalformat = GL_RGBA32I, GLenum texturedims = GL_TEXTURE_2D_MULTISAMPLE, GLsizei w=0, GLsizei h=0) : refs(new uint(1)), buf(genBufferId()), texturedims(texturedims), samples(samples), internalformat(internalformat), width(genWidth(w)), height(genHeight(h)){
		bind();
		if(texturedims == GL_TEXTURE_2D_MULTISAMPLE || texturedims == GL_PROXY_TEXTURE_2D_MULTISAMPLE){
			glTexImage2DMultisample(texturedims, samples, internalformat, width, height, GL_FALSE);
			Log::GLErrors("glTexImage2DMultisample(texturedims, samples, internalformat, width, height, GL_FALSE);");
		}
		if(texturedims == GL_TEXTURE_2D || texturedims == GL_PROXY_TEXTURE_2D){

			int format = GL_RGBA;
			int type = GL_FLOAT;

			if(internalformat==GL_DEPTH_COMPONENT||internalformat==GL_DEPTH_COMPONENT16||internalformat==GL_DEPTH_COMPONENT24||internalformat==GL_DEPTH_COMPONENT32||internalformat==GL_DEPTH_COMPONENT32F){
				format = GL_DEPTH_COMPONENT;
			}
			if(internalformat==GL_R16UI){
				format = GL_RED;
				type = GL_UNSIGNED_SHORT;
			}
			if(internalformat==GL_R32I){
				format =  GL_RGBA;
				type = GL_INT;
			}
			if(internalformat==GL_RGB8){
				format = GL_RGB;
				type = GL_UNSIGNED_BYTE;
			}

			glTexImage2D(texturedims, 0, internalformat, width, height, 0, format, type, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
	}
	Texture(const Texture &rhs) : refs(rhs.refs), buf(rhs.buf), samples(rhs.samples), internalformat(rhs.internalformat), width(rhs.width), height(rhs.height), texturedims(rhs.texturedims) {
		(*refs)++;
	}
	~Texture(){
		if(--*refs == 0){
			delete refs;
			glDeleteTextures(1, &buf);
			Log::GLErrors("glDeleteTextures(1, &buf);");
		}
	}
};
