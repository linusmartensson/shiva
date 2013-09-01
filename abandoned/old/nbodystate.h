#pragma once
#include"stdafx.h"
#include"clengine.h"
#include"varray.h"
#include"shaders.h"

class nbodystate : public shiva::state {
	static const unsigned int particleCount = 7680;
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
		glm::mat4 rot(1.0);
		rot = glm::rotate(rot, (float)0.0f, glm::vec3(0,0,1));
		glUniformMatrix4fv(p.uniform("rot"), 1, GL_FALSE, glm::value_ptr(rot));
		glClear(GL_COLOR_BUFFER_BIT);
		vb.draw(0,pts.size()/4, 1, GL_POINTS);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDisable(GL_BLEND);
	}
	bool stateUpdate(){
		vert.cloneContent(vert2);
		color.cloneContent(color2);
		return true;
	}
	void update(){
		clprogram.set("gravitymap", 2, float(shiva::core::TICK_TIME)*0.001f);
		const unsigned int particles[] = {particleCount};
		clprogram.run("gravitymap", particleCount);
	}

	nbodystate() : 
		clprogram(loadFile("statedata/tests/nbodystate/test.cl"), "-cl-fast-relaxed-math -cl-mad-enable"), 
		vert(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
		color(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
		color2(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
		vert2(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY) {
		
		clprogram.genkernel("gravitymap");

		Shader	sv(GL_VERTEX_SHADER, loadFile("statedata/tests/nbodystate/minimal.vert")), 
			sg(GL_GEOMETRY_SHADER, loadFile("statedata/tests/nbodystate/minimal.geom")), 
			sf(GL_FRAGMENT_SHADER, loadFile("statedata/tests/nbodystate/minimal.frag"));

		p.attach(&sv);
		p.attach(&sg);
		p.attach(&sf);
		p.link();


		for(size_t i=0;i<particleCount;++i){
			pts.push_back(((rand()%10000-5000)/50.0f));
			pts.push_back(((rand()%10000-5000)/50.0f));
			pts.push_back(((rand()%10000-5000)/50.0f));
			pts.push_back(1.0f);
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

		clprogram.set("gravitymap", 0, vert2);
		clprogram.set("gravitymap", 1, color2);
	}
};
