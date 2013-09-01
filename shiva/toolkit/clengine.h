#pragma once

#include"stdafx.h"

#include"buffers.h"
#include"log.h"
#include"shiva.h"
#include<unordered_map>
#include"varray.h"
/*Due to OpenCL 1.0 limitations, make sure to only use CLProgram in one thread, no matter which instance it is. OpenCL 1.0 is NOT thread safe even with separate contexts. :(*/
class CLProgram{
	class CLContext{
		cl_context _context;
		cl_command_queue _queue;
		cl_device_id _device;
		std::unordered_map<uint, cl_mem> glbuffers;
		std::unordered_map<uint, cl_mem> gltextures;
		std::vector<cl_mem> glbufferlinear;
		std::unordered_map<void*, cl_mem> buffers;
	public:
		CLContext(){
			cl_platform_id platform[10];
			cl_uint platform_count;
			Log::CLErrors(clGetPlatformIDs(10, platform, &platform_count), "clGetPlatformIDs(1, &platform, &platform_count)");
			Log::info()<<"Platform count: "<<platform_count<<std::endl;
			cl_device_id devices[10];
			cl_uint device_count;
			Log::CLErrors(clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_GPU, 10, devices, &device_count), "clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &devices, 0)");
			Log::info()<<"Device count: "<<device_count<<std::endl;
			_device = devices[0];
			cl_context_properties props[] = 
			{
				CL_GL_CONTEXT_KHR, (cl_context_properties)shiva::core::primaryRenderingContext(), 
				CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), 
				CL_CONTEXT_PLATFORM, (cl_context_properties)platform[0], 
				0
			};
			cl_int err;
			_context = clCreateContext(props, 1, &devices[0], 0, 0, &err);
			Log::CLErrors(err, "_context = clCreateContext(props, 1, &devices, 0, 0, &err);");
			_queue = clCreateCommandQueue(_context, devices[0], 0, &err);
			Log::CLErrors(err, "_queue = clCreateCommandQueue(_context, devices, 0, &err);");
		}
		~CLContext(){
			for(auto i=glbuffers.begin(); i!=glbuffers.end(); ++i){
				Log::CLErrors(clReleaseMemObject(i->second), "clReleaseMemObject(*i)");
			}
			for(auto i=gltextures.begin(); i!=gltextures.end(); ++i){
				Log::CLErrors(clReleaseMemObject(i->second), "clReleaseMemObject(*i)");
			}
			for(auto i=buffers.begin(); i!=buffers.end(); ++i){
				Log::CLErrors(clReleaseMemObject(i->second), "clReleaseMemObject(*i)");
			}
			glbuffers.clear();
			gltextures.clear();
			buffers.clear();
			Log::CLErrors(clReleaseCommandQueue(_queue), "clReleaseCommandQueue(_queue)");
			Log::CLErrors(clReleaseContext(_context), "clReleaseContext(_context)");
		}
		cl_context& context(){
			return _context;
		}
		cl_command_queue& queue(){
			return _queue;
		}
		cl_device_id device(){
			return _device;
		}

		//memflags is ignored after initial creation!
		template<typename ContentType>
		cl_mem buffer(const Buffer<ContentType> &b, cl_mem_flags memflags = CL_MEM_READ_WRITE){
			cl_int err;
			auto i = glbuffers.find(b.buf);
			if(i != glbuffers.end())
				return i->second;
			cl_mem mm = clCreateFromGLBuffer(context(), memflags, b.buf, &err);
			glbufferlinear.push_back(mm);
			glbuffers[b.buf] = mm;
			Log::CLErrors(err, "glbuffers[b.buf] = (clCreateFromGLBuffer(context->context(), memflags, b.buf, &err));");
			return mm;
		}
		
		//memflags is ignored after initial creation!
		cl_mem buffer(const Texture &b, cl_mem_flags memflags = CL_MEM_READ_WRITE){
			cl_int err;
			auto i = gltextures.find(b.buf);
			if(i != gltextures.end())
				return i->second;
			Log::info()<<"adding texture: "<<b.buf<<"..."<<std::endl;
			cl_mem mm = clCreateFromGLTexture2D(context(), memflags, b.texturedims, 0, b.buf, &err);
			Log::info()<<"texture cl_mem: "<<mm<<"..."<<std::endl;
			glbufferlinear.push_back(mm);
			gltextures[b.buf] = mm;
			Log::CLErrors(err, "gltextures[b.buf] = (clCreateFromGLTexture2D(context->context(), memflags, b.texturedims, 0, b.buf, &err));");
			return mm;
		}

		//size and memflags is ignored after initial creation!
		template<typename ContentType>
		cl_mem buffer(ContentType *data, const size_t size, cl_mem_flags memflags = CL_MEM_READ_WRITE){
			cl_int err;
			auto i = buffers.find(data);
			if(i != buffers.end())
				return i->second;
			buffers[data] = (clCreateBuffer(context(), memflags, size*sizeof(ContentType), data, &err));
			Log::CLErrors(err, "buffers[data] = (clCreateBuffer(context->context(), memflags, size*sizeof(ContentType), data, &err));");
			return buffers[data];
		}
		void begin(){
			if(glbufferlinear.size()){
				glFinish();
				Log::CLErrors(clEnqueueAcquireGLObjects(queue(), glbufferlinear.size(), &(glbufferlinear)[0], 0,0,0), "clEnqueueAcquireGLObjects(context->queue(), glbuffers->size(), &(*glbuffers)[0], 0,0,0)");
				Log::GLErrors("glFinish()");
			}
		}
		void end(){
			if(glbufferlinear.size()){
				Log::CLErrors(clEnqueueReleaseGLObjects(queue(), glbufferlinear.size(), &(glbufferlinear)[0], 0,0,0), "clEnqueueReleaseGLObjects(context->queue(), glbuffers->size(), &(*glbuffers)[0], 0,0,0)");
			}
			Log::CLErrors(clFinish(queue()), "clFinish(context->queue())");
		}
		void release(cl_mem &m){
			for(auto i = glbuffers.begin(); i!=glbuffers.end();++i){
				if(i->second == m){
					glbuffers.erase(i);
					glbufferlinear.erase(std::find(glbufferlinear.begin(), glbufferlinear.end(), m));
					Log::CLErrors(clReleaseMemObject(m), "clReleaseMemObject(m)");
					break;
				}
			}
			for(auto i = gltextures.begin(); i!=gltextures.end();++i){
				if(i->second == m){
					gltextures.erase(i);
					glbufferlinear.erase(std::find(glbufferlinear.begin(), glbufferlinear.end(), m));
					Log::CLErrors(clReleaseMemObject(m), "clReleaseMemObject(m)");
					break;
				}
			}
			for(auto i = buffers.begin(); i!=buffers.end(); ++i){
				if(i->second == m){
					buffers.erase(i);
					Log::CLErrors(clReleaseMemObject(m), "clReleaseMemObject(m)");
					break;
				}
			}
		}
		template<typename T>
		void erase(Buffer<T> &t){
			if(glbuffers.find(t.buf) != glbuffers.end()){
				release(glbuffers[t.buf]);
			}
		}
		void erase(Texture &t){
			if(gltextures.find(t.buf) != gltextures.end()){
				release(gltextures[t.buf]);
			}
		}
		template <typename T>
		void erase(T *t){
			if(buffers.find(t) != buffers.end()){
				buffers.erase(buffers.find(t));	
			}
		}
	};
	cl_program *_program;
	std::unordered_map<std::string, cl_kernel> *_kernels;
	static CLContext *context;
	static size_t usage;
	size_t *refs;

public:
	CLProgram(std::string src, std::string compileropts="-cl-fast-relaxed-math -cl-mad-enable") : refs(new size_t(1)), _program(new cl_program()), _kernels(new std::unordered_map<std::string, cl_kernel>()) {
		if(usage++ == 0){
			context = new CLContext();
		}
		const char *c = src.c_str();
		size_t l = src.length();
		cl_int err;
		Log::info()<<"Building OpenCL program..."<<std::endl;
		*_program = clCreateProgramWithSource(context->context(), 1, &c, &l, &err);
		Log::CLErrors(err, "*_program = clCreateProgramWithSource(context->context(), 1, &c, &l, &err);");
		Log::info()<<"Created OpenCL program!"<<std::endl;
		try{
			Log::CLErrors(clBuildProgram(*_program, 0,0, compileropts.c_str(), 0,0), "clBuildProgram(*_program, 0,0, \"\", 0,0);");
		} catch (Log::CLException){
			char buf[2048];
			size_t length;
			clGetProgramBuildInfo(*_program, context->device(), CL_PROGRAM_BUILD_LOG, sizeof(buf), buf, &length);
			Log::errlog()<<buf<<std::endl;
			throw Log::CLException();
		}
		Log::info()<<"Built OpenCL program!"<<std::endl;
	}
	CLProgram(const CLProgram &rhs) : _program(rhs._program), _kernels(rhs._kernels), refs(rhs.refs) {
		(*refs)++;
	}
	void genkernel(std::string func){
		cl_int err;
		(*_kernels)[func] = clCreateKernel(*_program, func.c_str(), &err);
		Log::CLErrors(err, "(*_kernels)[func] = clCreateKernel(*_program, func.c_str(), &err);");
	}

	template<typename ContentType>
	cl_mem buffer(ContentType *data, const size_t size, cl_mem_flags memflags = CL_MEM_READ_WRITE){
		return context->buffer(data, size, memflags);
	}
	
	template<typename ContentType>
	cl_mem buffer(ContentType &data, cl_mem_flags memflags = CL_MEM_READ_WRITE){
		return context->buffer(data, memflags);
	}

	template<typename T>
	void erase(T *t){
		context->erase(t);
	}
	template<typename T>
	void erase(T &t){
		context->erase(t);
	}

	void begin(){
		context->begin();
	}
	void end(){
		context->end();
	}
	~CLProgram(){
		if(--*refs == 0){
			for(auto i=_kernels->begin(); i!=_kernels->end(); ++i){
				Log::CLErrors(clReleaseKernel(i->second), "clReleaseKernel(i->second)"); 
			}
			Log::CLErrors(clReleaseProgram(*_program), "clReleaseProgram(*_program)");
			delete _kernels;
			delete _program;

			delete refs;
			if(--usage == 0){
				delete context; 
				context = 0;
			}
		}
	}

	bool rebuild(std::string src, std::string compileropts="-cl-fast-relaxed-math -cl-mad-enable"){
		const char *c = src.c_str();
		size_t l = src.length();
		cl_int err;
		cl_program newprogram;
		newprogram = clCreateProgramWithSource(context->context(), 1, &c, &l, &err);
		try{	
			Log::CLErrors(err, "newprogram = clCreateProgramWithSource(context->context(), 1, &c, &l, &err);");
		} catch (...) {
			return false;
		}
		try{
			Log::CLErrors(clBuildProgram(newprogram, 0,0, compileropts.c_str(), 0,0), "clBuildProgram(newprogram, 0,0, \"\", 0,0);");
		} catch (...) {
			char buf[2048];
			size_t length;
			clGetProgramBuildInfo(*_program, context->device(), CL_PROGRAM_BUILD_LOG, sizeof(buf), buf, &length);
			Log::errlog()<<buf<<std::endl;
			return false;
		}
		for(auto i=_kernels->begin(); i!=_kernels->end(); ++i){
			Log::CLErrors(clReleaseKernel(i->second), "clReleaseKernel(i->second)"); 
		}
		Log::CLErrors(clReleaseProgram(*_program), "clReleaseProgram(*_program)");
		_kernels->clear();
		
		*_program = newprogram;
		return true;

	}

	cl_command_queue& queue(){
		return context->queue();
	}
	cl_kernel& kernel(std::string func){
		return (*_kernels)[func];
	}
	void run(cl_kernel &k, const cl_uint *dimSzs, cl_uint dims, const cl_uint *localDimSzs, bool guard = true){
		if(guard) context->begin();
		clEnqueueNDRangeKernel(queue(), k, dims, NULL, dimSzs, localDimSzs, 0,0,0 );
		if(guard) context->end();
	}
	void run(cl_kernel &k, cl_uint sz, bool guard = true){
		sz = sz<=0?1:sz;
		run(k, &sz, 1, NULL, guard);
	}
	void run(std::string k, const cl_uint *dimSzs, cl_uint dims, const cl_uint *localDimSzs, bool guard = true){
		run(kernel(k), dimSzs, dims, localDimSzs, guard);
	}
	void run(std::string k, cl_uint sz, bool guard = true){
		run(kernel(k), sz, guard);
	}
	template<typename ContentType>
	void set(cl_kernel &k, cl_uint index, const Buffer<ContentType> &t){
		cl_mem mm = context->buffer(t);
		Log::CLErrors(clSetKernelArg(k, index, sizeof(cl_mem), &mm));
	}
	template<typename ContentType>
	void set(std::string func, cl_uint index, const Buffer<ContentType> &t){
		set(kernel(func), index, t);
	}
	void set(cl_kernel &k, cl_uint index, const Texture &t){
		Log::info()<<"entered set for Texture... "<<std::endl;
		cl_mem mm = context->buffer(t);
		Log::info()<<mm<<std::endl;
		Log::CLErrors(clSetKernelArg(k, index, sizeof(cl_mem), &mm), "clSetKernelArg(k, index, sizeof(cl_mem), &mm)");
		Log::info()<<"exited set for Texture... "<<std::endl;
	}
	void set(std::string func, cl_uint index, const Texture &t){
		set(kernel(func), index, t);
	}
	template<typename T>
	void set(cl_kernel &k, cl_uint index, const T &t){
		Log::CLErrors(clSetKernelArg(k, index, sizeof(T), &t), "clSetKernelArg((*_kernels)[func], index, sizeof(T), &t)");
	}
	template<typename T>
	void set(std::string func, cl_uint index, const T &t){
		set(kernel(func), index, t);	
	}

	//Warning: Copying data to and from the cl_mem buffer is not currently supported:
	//clEnqueueWriteBuffer(clprogram->queue(), clprogram->buffer(d, buffersize), CL_TRUE, 0, sizeof(T)*buffersize, d, 0, NULL, NULL);
	//clprogram->run("update", 1);
	//clEnqueueReadBuffer(clprogram->queue(), clprogram->buffer(d, buffersize), CL_TRUE, 0, sizeof(T)*buffersize, d, 0, NULL, NULL);
	template<typename ContentType>
	void set(cl_kernel &k, cl_uint index, const size_t size, ContentType *data){
		cl_mem mm = context->buffer(data, size);
		Log::CLErrors(clSetKernelArg(k, index, sizeof(cl_mem), &mm));
	}
	template<typename T>
	void setLocal(cl_kernel &k, cl_uint index, size_t sz){
		Log::CLErrors(clSetKernelArg(k, index, sizeof(T)*sz, NULL), "clSetKernelArg((*_kernels)[func], index, sizeof(T)*sz, NULL)");
	}
	template<typename ContentType>
	void set(std::string k, cl_uint index, const size_t size, ContentType *data){
		set(kernel(k), index, size, data);
	}
	template<typename T>
	void setLocal(std::string func, cl_uint index, size_t sz){
		setLocal(kernel(func), index, sz);
	}

};
/*
class SimpleCLProgram{
	struct programdata{
		std::unordered_map<std::string, CLProgram*> programs;
		~programdata(){
			for(auto i = programs.begin(); i != programs.end(); ++i){
				delete i->second;
			}
		}
	};
	static programdata programs;
	static CLProgram* generate(std::string p){
		if(programs.programs.find(p) == programs.programs.end()){
			programs.programs[p] = new CLProgram(p);
		}
		return new CLProgram(*(programs.programs[p]));
	}

	Buffer<GLfloat> *vert, *pvert;
	CLProgram *clprogram;
	int *pcount, *count;
	int *refs;
	int *maxcount;
public:

	SimpleCLProgram(std::string src, int mc) : 
	  refs(new int(1)), 
		  pcount(new int(0)), 
		  count(new int(0)), 
		  maxcount(new int(mc)), 
		  clprogram(generate(std::string("__kernel void update(__global float4* pos, float count, float maxcount, float acctime, float poff)")+
		  "{int i = get_global_id(0)+poff*maxcount;float f = (float)i/maxcount;float x=0;float y=0;float z=0; float w=0;"+src+
		  "pos[get_global_id(0)] = (float4)(x,y,z, w);}__kernel void zero(__global float4* pos){	pos[get_global_id(0)] = (float4)(0.0,0.0,0.0,0.0); }")), 
		  vert(new Buffer<GLfloat>(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY)), 
		  pvert(new Buffer<GLfloat>(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY)){
			  pvert->data((*maxcount)*4);
			  clprogram->genkernel("update");
			  clprogram->genkernel("zero");
			  clprogram->set("zero", 0, *pvert);
			  clprogram->run("zero", *pcount);
	  }
	  SimpleCLProgram(const SimpleCLProgram &rhs) : refs(rhs.refs), pcount(rhs.pcount), count(rhs.count), maxcount(rhs.maxcount), clprogram(rhs.clprogram), vert(rhs.vert), pvert(rhs.pvert){
		  (*refs)++;
	  }
	  ~SimpleCLProgram(){
		  if(--*refs == 0){
			  delete refs, delete pcount, delete count, delete maxcount, delete clprogram, delete vert, delete pvert;
		  }
	  }
	  void begin(){
		  clprogram->begin();
	  }
	  void end(){
		  clprogram->end();
	  }
	  bool stateUpdate(){
		  vert->cloneContent(*pvert);
		  *count = *pcount;
		  return true;
	  }
	  void limit(int c){
		  *pcount = c>*maxcount?*maxcount:(c<0?0:c);
	  }
	  int limit(){
		  return *pcount;
	  }
	  void maxLimit(int c){
		  *maxcount = c;
		  pvert->data((*maxcount)*4);
		  *pcount = c>*maxcount?*maxcount:c;
	  }
	  int maxLimit(){
		  return *maxcount;
	  }
	  void update(bool guarded = true){
		  auto k = clprogram->kernel("update");
		  clprogram->set(k, 0, *pvert);
		  clprogram->set(k, 1, (float)*pcount);
		  clprogram->set(k, 2, (float)*maxcount);
		  clprogram->set(k, 3, shiva::core::time()*0.001f);
		  clprogram->set(k, 4, 0);
		  clprogram->run(k, *pcount, guarded);
	  }

	  float d[1024];

	  template<int i>
	  std::vector<glm::vec3> subset(float pos, bool guarded = true){
		  auto k = clprogram->kernel("update");

		  clprogram->set(k, 0, i*4, d);
		  clprogram->set(k, 1, (float)*pcount);
		  clprogram->set(k, 2, (float)*maxcount);
		  clprogram->set(k, 3, shiva::core::physicsTime()*0.001f);
		  clprogram->set(k, 4, pos);
		  auto b = clprogram->buffer(d,1024);
		  clEnqueueWriteBuffer(clprogram->queue(), b, CL_TRUE, 0, sizeof(float)*4*i, d, 0, NULL, NULL);
		  clprogram->run(k, i, guarded);
		  clEnqueueReadBuffer(clprogram->queue(), b, CL_TRUE, 0, sizeof(float)*4*i, d, 0, NULL, NULL);
		  std::vector<glm::vec3> v;
		  for(int j=0;j<i;++j){
			  v.push_back(glm::vec3(d[j*4],d[j*4+1],d[j*4+2]));
		  }
		  return v;
	  }
	  Buffer<GLfloat> vertices(){
		  return *vert;
	  }
};

class StatelessCLProgram{
	struct programdata{
		std::unordered_map<std::string, CLProgram*> programs;
		~programdata(){
			for(auto i = programs.begin(); i != programs.end(); ++i){
				delete i->second;
			}
		}
	};
	static programdata programs;
	static CLProgram* generate(std::string p){
		if(programs.programs.find(p) == programs.programs.end()){
			programs.programs[p] = new CLProgram(p);
		}
		return new CLProgram(*(programs.programs[p]));
	}
	bool *refreshed, *needRefresh;
	cl_mem *bn, *bt, *bv;
	Buffer<GLfloat> *vert, *pvert;
	VArray *va;
	CLProgram *clprogram;
	int *refs;
	int *maxcount;
	std::vector<GLfloat> *csvert, *cstang, *csnorm;
public:
	  Buffer<GLfloat> vertices(){
		  return *vert;
	  }
	  VArray *varray(){
		  return va;
	  }
	void rebuild(std::string src, std::string funcs=""){
		 if(clprogram->rebuild(funcs+std::string("__kernel void update(__global float4* pos, float count, float maxcount, float acctime, __global float4* tang, __global float4* posb, __global float4* norm)")+
		  "{int i = get_global_id(0);float f = (float)i/maxcount;float x=0, y=0, z=0, w=0, xt=0, yt=0, zt=0,xn=0,yn=0,zn=0;"+src+
		  ";pos[get_global_id(0)] = (float4)(x,y,z, w);posb[get_global_id(0)] = (float4)(x,y,z, w);norm[get_global_id(0)] = (float4)(xn,yn,zn, 0); tang[get_global_id(0)] = (float4)(xt,yt,zt,0);}__kernel void zero(__global float4* pos){	pos[get_global_id(0)] = (float4)(0.0,0.0,0.0,0.0); }")){
		
			clprogram->erase(cstang);
			clprogram->erase(csvert);
			clprogram->erase(csnorm);
		
			pvert->data((*maxcount)*4);
			clprogram->genkernel("update");
			clprogram->genkernel("zero");
			  
			clprogram->set("zero", 0, *pvert);
			clprogram->run("zero", *maxcount);

			*bt = clprogram->buffer(&(*cstang)[0],4**maxcount);
			*bn = clprogram->buffer(&(*csnorm)[0],4**maxcount);
			*bv = clprogram->buffer(&(*csvert)[0],4**maxcount);
			clprogram->set("zero", 0, 4**maxcount, &(*csvert)[0]);
			clprogram->run("zero", *maxcount);
			clEnqueueReadBuffer(clprogram->queue(), *bv, CL_TRUE, 0, sizeof(float)*4**maxcount, &(*csvert)[0], 0, NULL, NULL);
			  
			clprogram->set("zero", 0, 4**maxcount, &(*cstang)[0]);
			clprogram->run("zero", *maxcount);
			clEnqueueReadBuffer(clprogram->queue(), *bt, CL_TRUE, 0, sizeof(float)*4**maxcount, &(*cstang)[0], 0, NULL, NULL);
			  
			clprogram->set("zero", 0, 4**maxcount, &(*csnorm)[0]);
			clprogram->run("zero", *maxcount);
			clEnqueueReadBuffer(clprogram->queue(), *bn, CL_TRUE, 0, sizeof(float)*4**maxcount, &(*csnorm)[0], 0, NULL, NULL);
			  
			  
			va->vertices(0, 4, vertices());
		}
	}
	void refresh(){*needRefresh = true;}
	StatelessCLProgram(std::string src, int mc, std::string funcs="") : 
	  refs(new int(1)),
		  maxcount(new int(mc)),
		  refreshed(new bool(true)),
		  bn(new cl_mem()),
		  bt(new cl_mem()),
		  bv(new cl_mem()),
		  va(new VArray()),
		  needRefresh(new bool(true)),
		  clprogram(generate(funcs+std::string("__kernel void update(__global float4* pos, float count, float maxcount, float acctime, __global float4* tang, __global float4* posb, __global float4* norm)")+
		  "{int i = get_global_id(0);float f = (float)i/maxcount;float x=0, y=0, z=0, w=0, xt=0, yt=0, zt=0,xn=0,yn=0,zn=0;"+src+
		  ";pos[get_global_id(0)] = (float4)(x,y,z, w);posb[get_global_id(0)] = (float4)(x,y,z, w);norm[get_global_id(0)] = (float4)(xn,yn,zn, 0); tang[get_global_id(0)] = (float4)(xt,yt,zt,0);}__kernel void zero(__global float4* pos){	pos[get_global_id(0)] = (float4)(0.0,0.0,0.0,0.0); }")), 
		  vert(new Buffer<GLfloat>(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY)), 
		  pvert(new Buffer<GLfloat>(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY)),
		  csvert(new std::vector<GLfloat>(*maxcount*4)),
		  cstang(new std::vector<GLfloat>(*maxcount*4)),
		  csnorm(new std::vector<GLfloat>(*maxcount*4)){
			  pvert->data((*maxcount)*4);
			  clprogram->genkernel("update");
			  clprogram->genkernel("zero");
			  
			  clprogram->set("zero", 0, *pvert);
			  clprogram->run("zero", *maxcount);

			  *bt = clprogram->buffer(&(*cstang)[0],4**maxcount);
			  *bn = clprogram->buffer(&(*csnorm)[0],4**maxcount);
			  *bv = clprogram->buffer(&(*csvert)[0],4**maxcount);
			  clprogram->set("zero", 0, 4**maxcount, &(*csvert)[0]);
			  clprogram->run("zero", *maxcount);
			  clEnqueueReadBuffer(clprogram->queue(), *bv, CL_TRUE, 0, sizeof(float)*4**maxcount, &(*csvert)[0], 0, NULL, NULL);
			  
			  clprogram->set("zero", 0, 4**maxcount, &(*cstang)[0]);
			  clprogram->run("zero", *maxcount);
			  clEnqueueReadBuffer(clprogram->queue(), *bt, CL_TRUE, 0, sizeof(float)*4**maxcount, &(*cstang)[0], 0, NULL, NULL);
			  
			  clprogram->set("zero", 0, 4**maxcount, &(*csnorm)[0]);
			  clprogram->run("zero", *maxcount);
			  clEnqueueReadBuffer(clprogram->queue(), *bn, CL_TRUE, 0, sizeof(float)*4**maxcount, &(*csnorm)[0], 0, NULL, NULL);
			  
			  
			  va->vertices(0, 4, vertices());
	  }
	  StatelessCLProgram(const StatelessCLProgram &rhs) : refs(rhs.refs), va(rhs.va), maxcount(rhs.maxcount), needRefresh(rhs.needRefresh), clprogram(rhs.clprogram), vert(rhs.vert), pvert(rhs.pvert), csvert(rhs.csvert), refreshed(rhs.refreshed), bt(rhs.bt), cstang(rhs.cstang), bn(rhs.bn), bv(rhs.bv), csnorm(rhs.csnorm){
		  (*refs)++;
	  }
	  ~StatelessCLProgram(){
		  if(--*refs == 0){
			  delete refs, delete maxcount, delete clprogram, delete va, delete vert, delete pvert, delete csvert, delete bv, delete refreshed, delete needRefresh, delete bt, delete cstang, delete bn, delete csnorm;
		  }
	  }
	  void begin(){
		  clprogram->begin();
	  }
	  void end(){
		  clprogram->end();
	  }
	  bool stateUpdate(){
		  vert->cloneContent(*pvert);
		  return true;
	  }
	  void maxLimit(int c){
		  *maxcount = c;
		  pvert->data((*maxcount)*4);
		  clprogram->erase(cstang);
		  cstang->resize(*maxcount*4);
		  clprogram->erase(csvert);
		  csvert->resize(*maxcount*4);
		  clprogram->erase(csnorm);
		  csnorm->resize(*maxcount*4);
		  *bv = clprogram->buffer(&(*csvert)[0],4**maxcount);
		  *bt = clprogram->buffer(&(*cstang)[0],4**maxcount);
		  *bn = clprogram->buffer(&(*csnorm)[0],4**maxcount);

		  va->vertices(0, 4, vertices());
	  }
	  int maxLimit(){
		  return *maxcount;
	  }
	  void update(bool guarded = true){
		  if(*needRefresh){
			  auto k = clprogram->kernel("update");
			  clprogram->set(k, 0, *pvert);
			  clprogram->set(k, 1, (float)*maxcount);
			  clprogram->set(k, 2, (float)*maxcount);
			  clprogram->set(k, 3, shiva::core::time()*0.001f);
			  clprogram->set(k, 4, 4**maxcount, &(*cstang)[0]);
			  clprogram->set(k, 5, 4**maxcount, &(*csvert)[0]);
			  clprogram->set(k, 6, 4**maxcount, &(*csnorm)[0]);
			  clprogram->run(k, *maxcount, guarded);
			  *refreshed = true;
			  *needRefresh = false;
		  }
	  }
	  void _ref(){
		  if(*refreshed){
			  clEnqueueReadBuffer(clprogram->queue(), *bv, CL_TRUE, 0, sizeof(float)*4**maxcount, &(*csvert)[0], 0, NULL, NULL);
			  clEnqueueReadBuffer(clprogram->queue(), *bt, CL_TRUE, 0, sizeof(float)*4**maxcount, &(*cstang)[0], 0, NULL, NULL);
			  clEnqueueReadBuffer(clprogram->queue(), *bn, CL_TRUE, 0, sizeof(float)*4**maxcount, &(*csnorm)[0], 0, NULL, NULL);
			  *refreshed = false;
		  }
	  }
	  int _pos(int pos){
		  return pos>=*maxcount?*maxcount-1:(pos<0?0:pos);
	  }
	  glm::vec3 vertex(int pos){
		  _ref();
		  pos = _pos(pos);
		  return glm::vec3((*csvert)[pos*4],(*csvert)[pos*4+1], (*csvert)[pos*4+2]);
	  }
	  glm::vec3 normal(int pos){
		  _ref();
		  pos = _pos(pos);
		  return glm::vec3((*csnorm)[pos*4],(*csnorm)[pos*4+1], (*csnorm)[pos*4+2]);
	  }
	  glm::vec3 tangent(int pos){
		  _ref();
		  pos = _pos(pos);
		  return glm::vec3((*cstang)[pos*4],(*cstang)[pos*4+1], (*cstang)[pos*4+2]);
	  }
};*/