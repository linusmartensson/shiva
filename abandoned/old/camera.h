#pragma once

#include"stdafx.h"
#include"state.h"
#include"controllables.h"



class camera : public shiva::state{
	glm::vec3 &trans;
	float &xd, &yd;
public:
	camera(std::string cameraname, glm::vec3 &trans, float &xd, float &yd, int fwd = GLFW_KEY_UP, int back = GLFW_KEY_DOWN, int left = GLFW_KEY_LEFT, int right = GLFW_KEY_RIGHT, int save=GLFW_KEY_ENTER, int reload=GLFW_KEY_F5, int savenext=GLFW_KEY_F11, int saveprev=GLFW_KEY_F12) : trans(trans), xd(xd), yd(yd) {
		

		std::string base = "interpolations/_builtin_camera_"+cameraname;
		std::string end = ".ip";


		shiva::controllable::make_control<glm::vec3>(
			sl, 
			base+"ws"+end, 
			trans, [&](glm::vec3 trans, int i){
				return trans + 
					(glm::vec4(0.f,0.f,i*100.f,1.0f)*
					glm::mat4_cast(
						glm::rotate(glm::quat(), yd, glm::vec3(1,0,0))*
						glm::rotate(glm::quat(), xd, glm::vec3(0,1,0)))
					).swizzle(glm::X, glm::Y, glm::Z);}, 
			shiva::controllable::key, 
			fwd, 
			back, true, save, reload, savenext, saveprev);
		shiva::controllable::make_control<glm::vec3>(
			sl, 
			base+"ad"+end, 
			trans, [&](glm::vec3 trans, int i){
				return trans + 
					(glm::vec4(i*100.f,0.f,0.f,1.0f)*
					glm::mat4_cast(
						glm::rotate(glm::quat(), yd, glm::vec3(1,0,0))*
						glm::rotate(glm::quat(), xd, glm::vec3(0,1,0)))
					).swizzle(glm::X, glm::Y, glm::Z);}, 
			shiva::controllable::key, 
			left, 
			right, true, save, reload, savenext, saveprev);
		shiva::controllable::make_control<float>(
			sl,
			base+"xd"+end,
			xd, [&](float xd, int i){
				return xd + i*0.3f;},
			shiva::controllable::mousex,
			0,0, false, save, reload, savenext, saveprev);
		shiva::controllable::make_control<float>(
			sl,
			base+"yd"+end,
			yd, [&](float yd, int i){
				return clamp(yd + i*0.3f,-90.f,90.f);},
			shiva::controllable::mousey,
			0,0, false, save, reload, savenext, saveprev);
	}

};