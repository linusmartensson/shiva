#pragma once

#include<string>
std::string loadFile(std::string fname, bool canFail = false);

template<typename T>
T clamp(const T &in, const T &low = 0.f, const T &high = 1.f){
	return std::min(std::max(in,low),high);
}

inline std::ostream &operator <<(std::ostream &os, const glm::vec3 &rhs){
	os<<rhs.x<<" "<<rhs.y<<" "<<rhs.z;
	return os;
}
inline std::istream &operator >>(std::istream &is, glm::vec3 &rhs){
	float a,b,c;
	is>>a>>b>>c;
	rhs = glm::vec3(a,b,c);
	return is;
}

#ifndef mkdir
#define mkdir(path) ::CreateDirectoryA(path, 0);
#endif