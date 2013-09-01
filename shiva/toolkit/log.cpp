#include"stdafx.h"

#include<fmod_errors.h>
#include"log.h"

namespace Log{
	
	std::ostream _voidlog(std::_Uninitialized);
#ifndef _NDEBUG
	std::ostream& errlog(){return std::cerr;}
	std::ostream& info(){return std::cout;}

	std::string describeCLError(cl_int err){
		switch (err) {
			case CL_SUCCESS:                            return "Success!";
			case CL_DEVICE_NOT_FOUND:                   return "Device not found.";
			case CL_DEVICE_NOT_AVAILABLE:               return "Device not available";
			case CL_COMPILER_NOT_AVAILABLE:             return "Compiler not available";
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "Memory object allocation failure";
			case CL_OUT_OF_RESOURCES:                   return "Out of resources";
			case CL_OUT_OF_HOST_MEMORY:                 return "Out of host memory";
			case CL_PROFILING_INFO_NOT_AVAILABLE:       return "Profiling information not available";
			case CL_MEM_COPY_OVERLAP:                   return "Memory copy overlap";
			case CL_IMAGE_FORMAT_MISMATCH:              return "Image format mismatch";
			case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "Image format not supported";
			case CL_BUILD_PROGRAM_FAILURE:              return "Program build failure";
			case CL_MAP_FAILURE:                        return "Map failure";
			case CL_INVALID_VALUE:                      return "Invalid value";
			case CL_INVALID_DEVICE_TYPE:                return "Invalid device type";
			case CL_INVALID_PLATFORM:                   return "Invalid platform";
			case CL_INVALID_DEVICE:                     return "Invalid device";
			case CL_INVALID_CONTEXT:                    return "Invalid context";
			case CL_INVALID_QUEUE_PROPERTIES:           return "Invalid queue properties";
			case CL_INVALID_COMMAND_QUEUE:              return "Invalid command queue";
			case CL_INVALID_HOST_PTR:                   return "Invalid host pointer";
			case CL_INVALID_MEM_OBJECT:                 return "Invalid memory object";
			case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "Invalid image format descriptor";
			case CL_INVALID_IMAGE_SIZE:                 return "Invalid image size";
			case CL_INVALID_SAMPLER:                    return "Invalid sampler";
			case CL_INVALID_BINARY:                     return "Invalid binary";
			case CL_INVALID_BUILD_OPTIONS:              return "Invalid build options";
			case CL_INVALID_PROGRAM:                    return "Invalid program";
			case CL_INVALID_PROGRAM_EXECUTABLE:         return "Invalid program executable";
			case CL_INVALID_KERNEL_NAME:                return "Invalid kernel name";
			case CL_INVALID_KERNEL_DEFINITION:          return "Invalid kernel definition";
			case CL_INVALID_KERNEL:                     return "Invalid kernel";
			case CL_INVALID_ARG_INDEX:                  return "Invalid argument index";
			case CL_INVALID_ARG_VALUE:                  return "Invalid argument value";
			case CL_INVALID_ARG_SIZE:                   return "Invalid argument size";
			case CL_INVALID_KERNEL_ARGS:                return "Invalid kernel arguments";
			case CL_INVALID_WORK_DIMENSION:             return "Invalid work dimension";
			case CL_INVALID_WORK_GROUP_SIZE:            return "Invalid work group size";
			case CL_INVALID_WORK_ITEM_SIZE:             return "Invalid work item size";
			case CL_INVALID_GLOBAL_OFFSET:              return "Invalid global offset";
			case CL_INVALID_EVENT_WAIT_LIST:            return "Invalid event wait list";
			case CL_INVALID_EVENT:                      return "Invalid event";
			case CL_INVALID_OPERATION:                  return "Invalid operation";
			case CL_INVALID_GL_OBJECT:                  return "Invalid OpenGL object";
			case CL_INVALID_BUFFER_SIZE:                return "Invalid buffer size";
			case CL_INVALID_MIP_LEVEL:                  return "Invalid mip-map level";
			default: return "Unknown";
		}
	}

	void FTErrors(FT_Error err, std::string infostring){
		if(err != 0){
			Log::errlog()<<"at "<<infostring<<" | FTERROR: "<<err<<std::endl;
			throw Log::FTException();
		}
	}
	

	void FMODErrors(FMOD_RESULT err, std::string infostring){
		if(err != FMOD_OK){
			Log::errlog()<<"at "<<infostring<<" | FMODERROR: "<<FMOD_ErrorString(err)<<std::endl;
		}
	}
	void CLErrors(cl_int err, std::string infostring){
		if(err != CL_SUCCESS){
			Log::errlog()<<"at "<<infostring<<" | CLERROR: "<<describeCLError(err)<<std::endl;
			throw Log::CLException();
		}
	}
	void GLErrors(std::string infostring){
		unsigned int err;
		unsigned int count = 0;
		while((err = glGetError()) != GL_NO_ERROR && count < 1000){
			count++;
			const GLubyte *e = gluErrorString(err);
			if(e != 0)
				Log::errlog()<<"at "<<infostring<<" | "<<(err==GL_OUT_OF_MEMORY?"FATAL GLERROR: ":"NONFATAL GLERROR: ")<<e<<std::endl;
			else
				Log::errlog()<<"at "<<infostring<<" | "<<"UNKNOWN GLERROR: "<<err<<std::endl;
			
		}
		if(count)throw Log::GLException();
	}

	void shaderCompiler(GLint shader)
	{
		// From OpenGL Shading Language 3rd Edition, p215-216
		// Display (hopefully) useful error messages if shader fails to compile
		
		int sz = 0, charsWritten = 0;
		GLchar *infoLog;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &sz);
		GLErrors("glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &sz);");

		if (sz > 0)
		{
			infoLog = new GLchar[sz];
			glGetShaderInfoLog(shader,sz, &charsWritten, infoLog);
			GLErrors("glGetShaderInfoLog(shader,sz, &charsWritten, infoLog);");
			std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
			delete [] infoLog;
		}
	}
	void programLinker(GLint shader)
	{
		int sz = 0, charsWritten = 0;
		GLchar *infoLog;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &sz);
		GLErrors("glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &sz);");

		if (sz > 0)
		{
			infoLog = new GLchar[sz];
			glGetProgramInfoLog(shader,sz, &charsWritten, infoLog);
			GLErrors("glGetProgramInfoLog(shader,sz, &charsWritten, infoLog);");
			std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
			delete [] infoLog;
		}
	}

#else
	std::ostream& errlog(){return _voidlog;}
	std::ostream& info(){return _voidlog;}
	void FTErrors(FT_Error err, std::string infostring){}
	void FMODErrors(FMOD_RESULT err, std::string infostring){}
	void CLErrors(cl_int err, std::string infostring){}
	void GLErrors(std::string infostring){}
	void shaderCompiler(GLint shader){}
	void programLinker(GLint shader){}
#endif


};