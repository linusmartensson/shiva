#pragma once

#include<iostream>
#include<CL/opencl.h>

#include<fmod_errors.h>


namespace Log{
	
	struct FTException{};
	struct CLException{};
	struct GLException{};
	std::ostream& errlog();
	std::ostream& info();
	void FMODErrors(FMOD_RESULT errcode, std::string infostring="");
	void FTErrors(FT_Error error, std::string infostring="");
	void CLErrors(cl_int err, std::string infostring="");
	void GLErrors(std::string infostring="");
	void shaderCompiler(GLint shader);
	void programLinker(GLint shader);

};