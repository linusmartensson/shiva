#pragma once

#include"stdafx.h"

inline GLenum getMatch(GLint){return GL_INT;}
inline GLenum getMatch(GLuint){return GL_UNSIGNED_INT;}
inline GLenum getMatch(GLbyte){return GL_BYTE;}
inline GLenum getMatch(GLubyte){return GL_UNSIGNED_BYTE;}
inline GLenum getMatch(GLshort){return GL_SHORT;}
inline GLenum getMatch(GLushort){return GL_UNSIGNED_SHORT;}
inline GLenum getMatch(GLfloat){return GL_FLOAT;}
inline GLenum getMatch(GLdouble){return GL_DOUBLE;}

/*Helper class to avoid template issues*/
class BufferBase{
protected:
	static uint copyReadBuf;
	static uint copyWriteBuf;

public:
	virtual uint size() const = 0;
	virtual ~BufferBase(){}
	static void reset(){
		copyReadBuf = copyWriteBuf = 0;
	}
};

template<typename ContentType>
class Buffer : public BufferBase{
	uint *elemcount;
	uint *refs;
	static uint createBufferId(){
		uint buf = 0;
		glGenBuffers(1, &buf);
		return buf;
	}
public:
	const uint buf;
	const GLenum type;
	const GLenum content;
	const GLenum usage;
	const uint elemsize;
	const uint elemsperitem;
	Buffer(GLenum type=GL_ARRAY_BUFFER, GLenum usage=GL_STATIC_DRAW, uint elemsperitem=1) : buf(createBufferId()), type(type), content(getMatch(ContentType())), usage(usage), elemsize(sizeof(ContentType)), elemcount(new uint(0)), refs(new uint(1)), elemsperitem(elemsperitem) {}
	Buffer(const Buffer<ContentType> &rhs) : buf(rhs.buf), elemcount(rhs.elemcount), refs(rhs.refs), type(rhs.type), content(rhs.content), usage(rhs.usage), elemsize(rhs.elemsize), elemsperitem(elemsperitem) {
		(*refs)++;
	}
	~Buffer(){
		if(--*refs == 0){
			delete refs;
			delete elemcount;
			glDeleteBuffers(1, &buf);
		}
	}
	uint size() const { return *elemcount; }					//Nr. of elements buffer is capable of storing. (Given that the buffer size has not been changed outside the interface.)
	void data(const uint size, const ContentType *data=0);		//Set contents of buffer using raw array.
	void data(const std::vector<ContentType> &data);			//Set contents of buffer using std::vector.
	
	void bind() const {	glBindBuffer(type, buf); }

	void subdata(const uint size, const uint start, const ContentType *data){
		if(copyWriteBuf != buf)
			glBindBuffer(GL_COPY_WRITE_BUFFER, buf);
		copyWriteBuf = buf;
		glBufferSubData(GL_COPY_WRITE_BUFFER, start*elemsize, size*elemsize, &data[0]);
	}

	//Clone content from t into this.
	void cloneContent(Buffer<ContentType> &t){
		if(type!=t.type || content!=t.content || elemsize!=t.elemsize){ //usage == t.usage is ignored, as they may very well differ!
			Log::errlog()<<"Buffer copy attempted on incompatible buffer types!"<<std::endl;
			return;
		}

		if(copyReadBuf != t.buf)
			glBindBuffer(GL_COPY_READ_BUFFER, t.buf);
		if(copyWriteBuf != buf)
			glBindBuffer(GL_COPY_WRITE_BUFFER, buf);
		
		copyReadBuf = t.buf;
		copyWriteBuf = buf;

		if(*t.elemcount != *elemcount){
			//Would it be useful to change the usage to something fitting since we are copying to it?
			glBufferData(GL_COPY_WRITE_BUFFER, (*t.elemcount)*elemsize, NULL, usage);
			*elemcount = *t.elemcount;
		}
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, (*elemcount)*elemsize);
	}
};




template<typename ContentType>
void Buffer<ContentType>::data(const uint size, const ContentType *data) {
	bind();
	if(size == *elemcount){
		
		if(copyWriteBuf != buf)
			glBindBuffer(GL_COPY_WRITE_BUFFER, buf);
		copyWriteBuf = buf;

		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, size*elemsize, data);
	} else
		glBufferData(type, size*elemsize, data, usage);
	*elemcount = size;
}
template<typename ContentType>
void Buffer<ContentType>::data(const std::vector<ContentType> &data) {
	bind();
	if(data.size() == *elemcount){
		
		if(copyWriteBuf != buf)
			glBindBuffer(GL_COPY_WRITE_BUFFER, buf);
		copyWriteBuf = buf;

		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, data.size()*elemsize, &data[0]);
	} else
		glBufferData(type, data.size()*elemsize, &data[0], usage); 
	*elemcount = data.size();
}