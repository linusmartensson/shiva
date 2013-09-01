#include"clengine.h"
#include<regex>

class ParseCL{

public:
	ParseCL(){}

	void parse(std::string src=""){
		std::regex kernel("__kernel [a-zA-Z0-9]+ [a-zA-Z0-9_-]+(\s|\n)*\((.|\n)*?\) {");
	}

};