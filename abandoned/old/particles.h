#pragma once
#include"stdafx.h"
#include"clengine.h"
#include"varray.h"
#include"shaders.h"

class particles : public shiva::state {
	static const unsigned int particleCount = 4000;
	VArray vb;
	Program p;
	Buffer<GLfloat> vert, color, vert2, color2;
	CLProgram clprogram;
	std::vector<GLfloat> pts;
	std::vector<GLfloat> colors;
public:
	void display(){
		glEnable(GL_BLEND);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE,GL_ONE);

		p.use();
		p.bindAttrib(0, "in_Position");
		p.bindAttrib(1, "color");
		glm::mat4 rot(1.0);
		float rt = shiva::core::renderTime();
		rot = glm::gtc::matrix_transform::infinitePerspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 0.1f)
			*glm::gtc::matrix_transform::lookAt(glm::vec3(13,1,12), glm::vec3(0,0,0), glm::vec3(0,1,0))
			*glm::rotate(rot, (float)rt*0.0011f, glm::vec3(1,0,0))
			*glm::rotate(rot, (float)rt*0.0017f, glm::vec3(0,1,0))
			*glm::rotate(rot, (float)rt*0.0013f, glm::vec3(0,0,1));
		glUniformMatrix4fv(p.uniform("rot"), 1, GL_FALSE, glm::value_ptr(rot));
		glClear(GL_COLOR_BUFFER_BIT);
		vb.draw(0,pts.size()/4, 1, GL_POINTS);
	}
	bool stateUpdate(){
		vert.cloneContent(vert2);
		color.cloneContent(color2);



		return true;
	}
	void update(){
		clprogram.set("flowparticles", 2, float(shiva::core::TICK_TIME)*0.001f);
		clprogram.set("flowparticles", 3, shiva::core::physicsTime()*0.001f);
		clprogram.run("flowparticles", particleCount);
	}
	particles(int argc, char **argv) : 
		clprogram(loadFile("particles.cl"), "-cl-fast-relaxed-math -cl-mad-enable"), 
		vert(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
		color(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
		color2(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
		vert2(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY) {
		
		clprogram.genkernel("flowparticles");

		Shader	sv(GL_VERTEX_SHADER, loadFile("particles.vert")), 
			sg(GL_GEOMETRY_SHADER, loadFile("minimal.geom")), 
			sf(GL_FRAGMENT_SHADER, loadFile("minimal.frag"));

		p.attach(&sv);
		p.attach(&sg);
		p.attach(&sf);
		p.link();


		for(size_t i=0;i<particleCount;++i){
			pts.push_back(((rand()%10000-5000)/500.0f));
			pts.push_back(((rand()%10000-5000)/500.0f));
			pts.push_back(((rand()%10000-5000)/500.0f));
			pts.push_back(((rand()%10000)/5000.0f));
			colors.push_back(0);
			colors.push_back(0);
			colors.push_back(0);
			colors.push_back(0);
		}
		vert2.data(pts);
		color2.data(colors);
		vb.vertices(0,4, vert);
		vb.vertices(1,4, color);	
		VArray::unbind();

		clprogram.set("flowparticles", 0, vert2);
		clprogram.set("flowparticles", 1, color2);	
		
	}
};
