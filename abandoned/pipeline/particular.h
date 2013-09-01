#pragma once
#include"stdafx.h"
#include"clengine.h"
#include"varray.h"
#include"shaders.h"
#include"state/common/camera.h"
#include"state/common/mixer.h"
#include"controllers.h"
#include"input.h"


class particular : public shiva::state {
	Instance<VArray> vb;
	Instance<Buffer<GLfloat>> screenposition, oldscreenposition, position, color, acceleration, state, rcolor, rscreenposition, roldscreenposition;
	Buffer<GLfloat> pcam, pcam2, pcam3;
	Program p;
	glm::mat4 cam;
	
	std::string clprogramFile;
	CLProgram clprogram;

	int currParticles;
	float speed;
	float timeFactor;
public:
	void display(){
		glDisable(GL_BLEND);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE,GL_ONE);

		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		p.use();
		p.bindAttrib(0, "position");
		p.bindAttrib(1, "position2");
		p.bindAttrib(2, "color");
		vb->draw(0,currParticles, 1, GL_POINTS);
		
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDisable(GL_BLEND);
		sl.display();
	}
	float to;
	void update(){
		clprogram.set("particleengine", 4, float(shiva::core::TICK_TIME)*0.001f*timeFactor);
		clprogram.set("particleengine", 5, (shiva::core::physicsTime())*1.0f);
		
		pcam.data(16, glm::value_ptr(cam));
		clprogram.set("particleengine", 6, pcam);

		clprogram.set("particleengine", 9, (float)shiva::core::pause());
		glm::mat4 rotcam = (glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)*
			glm::mat4_cast(glm::rotate(glm::quat(), yd, glm::vec3(1,0,0))*glm::rotate(glm::quat(), xd, glm::vec3(0,1,0))));
		
		glm::vec4 ptrans(trans*speed, 0.0);
		pcam2.data(16, glm::value_ptr(rotcam));
		pcam3.data(4, glm::value_ptr(ptrans));

		clprogram.set("particleengine", 10, pcam2);
		clprogram.set("particleengine", 11, pcam3);
		clprogram.run("particleengine", currParticles);
		sl.update();
	}

	glm::vec3 &trans;
	float &xd, &yd;
	
	std::string frage;
	std::string geome;
	std::string verte;

	void restart(){
			p.rebuild(frage, geome, verte);
			clprogram.rebuild(loadFile(clprogramFile), "-cl-fast-relaxed-math -cl-mad-enable");
			clprogram.genkernel("particleengine");
			clprogram.set("particleengine", 0, *position);
			clprogram.set("particleengine", 1, *acceleration);
			clprogram.set("particleengine", 2, *state);
			clprogram.set("particleengine", 3, *color);
			clprogram.set("particleengine", 7, *screenposition);
			clprogram.set("particleengine", 8, *oldscreenposition);
			
	}
	particular(
		unsigned int particleCount, 
		Instance<VArray> vb,
		Instance<Buffer<GLfloat>> screenposition, 
		Instance<Buffer<GLfloat>> oldscreenposition,
		Instance<Buffer<GLfloat>> position, 
		Instance<Buffer<GLfloat>> color, 
		Instance<Buffer<GLfloat>> acceleration, 
		Instance<Buffer<GLfloat>> state, 
		Instance<Buffer<GLfloat>> rcolor,
		Instance<Buffer<GLfloat>> rscreenposition,
		Instance<Buffer<GLfloat>> roldscreenposition,
		std::string clprogramFile,
		glm::vec3 &trans,
		float &xd, float &yd,
		std::string frage,
		std::string geome,
		std::string verte
		) :
		clprogramFile("builtins/tools.cl " + clprogramFile + " statedata/pipeline/particular/particleengine.cl"),
		clprogram(loadFile("builtins/tools.cl " + clprogramFile + " statedata/pipeline/particular/particleengine.cl"), "-cl-fast-relaxed-math -cl-mad-enable"), 	
		position(position), 
		color(color), 
		acceleration(acceleration),
		state(state),
		screenposition(screenposition), 
		oldscreenposition(oldscreenposition),
		rcolor(rcolor),
		rscreenposition(rscreenposition),
		roldscreenposition(roldscreenposition),
		vb(vb),
		trans(trans),
		xd(xd),
		yd(yd),
		pcam(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
		pcam2(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY), 
		pcam3(GL_ARRAY_BUFFER, GL_DYNAMIC_COPY),
		frage(frage), verte(verte), geome(geome) {

		currParticles = particleCount;
		speed=0.2;
		timeFactor = 1.0f;
		
		sl.add((new onkey('Q'))->add([&]{restart();}, false));
		sl.add((new onresources)->add([&]{restart();}, true));
		mixer *m = new mixer(clprogramFile);
		m->add<float>(0, "timeTransform", timeFactor, [](float f, int i){ return f+i*0.1f;}, 1.0f);
		sl.add(m);

		auto camera = new shiva::controls::tcontrol<shiva::pdata<glm::mat4>>();
		sl.add(camera);
		camera->add<glm::mat4>([&]{
			return glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), yd, glm::vec3(1,0,0))*glm::rotate(glm::quat(), xd, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), trans*speed))
				;
		}, cam, t(1), t(99999));

	}
};
