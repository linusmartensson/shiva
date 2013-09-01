#pragma once
#include"buffers.h"

class VArray{
private:
	uint va;
	GLenum *indicesFormat;
	BufferBase **indexBuffer;
	uint *refs;
	static uint current;
public:
	static void reset(){
		current = 0;
	}
	VArray() : indicesFormat(new GLenum(0)), indexBuffer(new BufferBase*(0)), refs(new uint(1)) {
		glGenVertexArrays(1, &va);
	}
	VArray(const VArray &rhs) : va(rhs.va), indicesFormat(rhs.indicesFormat), refs(rhs.refs), indexBuffer(rhs.indexBuffer) {
		(*refs)++;
	}
	~VArray(){
		if(--*refs == 0){
			if(current == va)
				current = 0; //unbind(); --> Calling glBindVertexArray(0) during cleanup caused errors, this is my fix. :(
			delete refs;
			delete indicesFormat;
			if(*indexBuffer){
				delete *indexBuffer;
			}
			delete indexBuffer;
			glDeleteVertexArrays(1, &va);
		}
	}
	void bind(){
		if(current == va) return;
		glBindVertexArray(va);
		current = va;
	}
	static void unbind(){
		if(current == 0) return;
		glBindVertexArray(0);
		current = 0;
	}
	void draw(const uint start, const uint size, const uint prim = 1, const GLenum drawType = GL_TRIANGLES){
		bind();
		Log::GLErrors("VArray::bind()");
		glDrawArraysInstanced(drawType, start, size, prim);
		Log::GLErrors("glDrawArraysInstanced(drawType, start, size, prim);");
	}
	void drawIndices(const uint count = 0, const uint prim = 1, const GLenum drawType = GL_TRIANGLES){
		bind();
		if(count == 0){
			if(*indicesFormat != 0 && *indexBuffer != 0){
				glDrawElementsInstanced(drawType, (*indexBuffer)->size(), *indicesFormat, 0, prim);
			}
		} else if(*indicesFormat != 0){
			glDrawElementsInstanced(drawType, count, *indicesFormat, 0, prim);
		}
		Log::GLErrors("glDrawElementsInstanced(drawType, start, size, prim);");
	}

	template<typename T>
	void vertices(const uint index, const uint size, Buffer<T> &buffer, const GLboolean normalize = GL_FALSE, const uint stride = 0, const uint offset = 0){
		bind();
		buffer.bind();
		glVertexAttribPointer(index, size, buffer.content, normalize, stride, (const GLvoid *)(offset*buffer.elemsize));
		glEnableVertexAttribArray(index);
	}

	template<typename T>
	void indices(Buffer<T> &buffer){
		bind();
		buffer.bind();
		*indicesFormat = buffer.content;
		if(*indexBuffer != 0) delete *indexBuffer;
		*indexBuffer = new Buffer<T>(buffer);
	}

	const void *indices(){
		return *indexBuffer;
	}
};