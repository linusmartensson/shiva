#include"stdafx.h"
#include"utils.h"
#include"log.h"
#include<sstream>

#pragma warning(disable: 4244)

std::string loadFile(std::string fn, bool canFail)
{
	std::ifstream::pos_type size;
	char * memblock;
	std::string text = "", f = "";

	std::istringstream iss(fn);
	
	while(iss >> f){
		std::string fname = "resources/";
		fname.append(f);

		// file read based on example in cplusplus.com tutorial
		std::ifstream file (fname.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
		if (file.is_open())
		{
			size = file.tellg();
			memblock = new char [size];
			file.seekg (0, std::ios::beg);
			file.read(memblock, size);
			file.close();
			Log::info() << "file " << fname << " loaded" << std::endl;
			text.append(std::string(memblock, size));
			delete [] memblock;
		}
		else
		{
			Log::errlog() << (canFail?"":"FATAL ERROR: ") << "Unable to open file " << fname << std::endl;
			if(!canFail) exit(1);
		} 
	}
	return text;
}