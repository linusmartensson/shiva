#pragma once

#include"stdafx.h"
#include"state.h"
#include"texture.h"
#include<functional>

class FrameBuffer{

	static uint genBufferId(){
		uint buf = 0;
		glGenFramebuffers(1, &buf);
		Log::GLErrors("glGenFramebuffers(1, &buf);");
		return buf;
	}

	uint *refs;
	std::stack<FrameBuffer*> *prev;
	std::vector<uint> *buffers;
	int *w, *h;
public:
	const uint buf;
	void bind(){
		glBindFramebuffer(GL_FRAMEBUFFER, buf);
		Log::GLErrors("glBindFramebuffer(GL_FRAMEBUFFER, buf);");
	}
private:
	static FrameBuffer *current;
	void begin(){
		prev->push(current);
		current = this;
		current->bind();
		//Log::errlog()<<glCheckFramebufferStatus(GL_FRAMEBUFFER)<<std::endl;
		if(buffers->size())
			glDrawBuffers(buffers->size(), &((*buffers)[0]));
		Log::GLErrors("glDrawBuffers(buffers->size(), &((*buffers)[0]));");
		glViewport(0,0,*w,*h);
	}
	void end(){
		current = prev->top();
		prev->pop();
		if(current != 0){
			current->bind();
			glViewport(0,0,*current->w, *current->h); 
			glDrawBuffers(current->buffers->size(), &((*(current->buffers))[0]));
			Log::GLErrors("glDrawBuffers(current->buffers->size(), &((*(current->buffers))[0]));");
		} else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDrawBuffer(GL_BACK);
			glViewport(0,0,shiva::core::width(), shiva::core::height());
		}
	}

public:

	FrameBuffer() : refs(new uint(1)), buf(genBufferId()), prev(new std::stack<FrameBuffer*>), buffers(new std::vector<uint>), w(new int(1)), h(new int(1)) {}
	FrameBuffer(const FrameBuffer &rhs) : refs(rhs.refs), buf(rhs.buf), prev(rhs.prev), buffers(rhs.buffers), w(rhs.w), h(rhs.h) {
		(*refs)++;
	}
	~FrameBuffer(){
		if(--*refs == 0){
			delete refs;
			glDeleteFramebuffers(1, &buf);
			Log::GLErrors("glDeleteFramebuffers(1, &buf);");
			delete buffers;
			delete prev;
			delete w; delete h;
		}
	}

	void colorbuffer(uint index, Texture &t){
		t.bind();
		*w = t.width;
		*h = t.height;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, buf);
		Log::GLErrors("glBindFramebuffer(GL_FRAMEBUFFER, buf);");
		glFramebufferTexture(GL_READ_FRAMEBUFFER, index+GL_COLOR_ATTACHMENT0, t.buf, 0);
		Log::GLErrors("glFramebufferTexture(GL_FRAMEBUFFER, index+GL_COLOR_ATTACHMENT0, t.buf, 0);");
		if(std::find(buffers->begin(), buffers->end(), index+GL_COLOR_ATTACHMENT0) == buffers->end())
			buffers->push_back(index+GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	void depthbuffer(Texture &t){
		t.bind();
		*w = t.width;
		*h = t.height;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, buf);
		Log::GLErrors("glBindFramebuffer(GL_FRAMEBUFFER, buf);");
		glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, t.buf, 0);
		Log::GLErrors("glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, t.buf, 0);");
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	
	template<typename T>
	void render(T *scene){
		begin();
		scene->display();
		Log::GLErrors("scene->display(); (This probably indicates an error either in a scene rendered in a framebuffer, or the framebuffer setup.)");
		end();
	}
	template<typename T>
	void render(T &f){
		begin();
		f();
		Log::GLErrors("f(); (This probably indicates an error either in a scene rendered in a framebuffer, or the framebuffer setup.)");
		end();
	}
	template<typename T>
	void render(T scene, std::vector<Texture> &colorbuffers, Texture &depthbuf){
		int j=0;
		for(auto i = colorbuffers.begin(); i!=colorbuffers.end(); ++i, ++j){
			colorbuffer(j, *i);
		}
		depthbuffer(depthbuf);
		render(scene);
	}
	template<typename T>
	void render(T scene, Texture &colorbuf, Texture &depthbuf){
		colorbuffer(0, colorbuf);
		depthbuffer(depthbuf);
		render(scene);
	}
};