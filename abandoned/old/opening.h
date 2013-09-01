#pragma once
#include"stdafx.h"
#include"state.h"
#include"fonts.h"
#include"texture.h"
#include"shaders.h"
#include"clengine.h"
#include"controllers.h"
#include"framebuffer.h"
#include"utils.h"
#include"lines.h"
#include"particles.h"
#include"interpolation.h"

using namespace shiva;
using namespace shiva::controls;
using namespace shiva::interpolate;
class opening : public shiva::state {
	shiva::statelist sl;
	Font font;
	Instance<FrameBuffer> fb;
	Instance<Texture> shivatext, stringtheory, hdroutput;
	float shivatextvisibility, stringtheoryvisibility;
	Instance<FSShader> textshader, hdrshader;
	Instance<Program> starfieldshader;
	Instance<SimpleCLProgram> starfield;
	Instance<VArray> starfieldva;
	glm::mat4 camera;
	float starlight;
	bool starfieldpath;
public:
	opening() : font("exmouth/exmouth_.ttf") {
		
		auto sflist = new statelist;

		auto maintimer = new timercontrol;
		sl.add(maintimer);

		
		auto *starfieldcamera = new tcontrol<data<glm::mat4>>;
		sflist->add(starfieldcamera);
		
		

		starfieldcamera->add<glm::mat4>([]{
			return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::rotate(glm::mat4(1.0f), (float)shiva::core::physicsTime()*0.013f, glm::vec3(0,0,1))
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(0.7,0,0), 
						glm::vec3(sin(shiva::core::physicsTime()*0.0004f),0,cos(shiva::core::physicsTime()*0.0004f)), 
						glm::vec3(0,1,0));
		}, camera, t(0), t(3));
		starfieldcamera->add<glm::mat4>([]{
			return glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::rotate(glm::mat4(1.0f), (float)shiva::core::physicsTime()*0.013f, glm::vec3(0,0,1))
				*glm::gtc::matrix_transform::lookAt(
						glm::vec3(0,0,0.5), 
						glm::vec3(cos(shiva::core::physicsTime()*0.0004f),0,sin(shiva::core::physicsTime()*0.0004f)), 
						glm::vec3(0,1,0));
		}, camera, t(3), t(500));

		//Show text
		sflist->add(make_data<float, data<float>>([&]{return clamp(0.001f*(core::physicsTime()-t(7)))*(1.f-clamp(0.001f*(core::physicsTime()-t(11))));}, shivatextvisibility));
		sflist->add(make_data<float, data<float>>([&]{return clamp(0.001f*(core::physicsTime()-t(11)))*(1.f-clamp(0.001f*(core::physicsTime()-t(14))));}, stringtheoryvisibility));

		auto linestate = new shiva::controls::control<state>;
		
		maintimer->listen(linestate);
		linestate->addevent(0);
		linestate->addevent(2);
		linestate->add(new statelist);
		linestate->add(new particles(0,0));
		linestate->add(new lines(0,0));

		sl.add(linestate);


		font.pixelsize(200);
		shivatext = font.render("ShivaX001");
		stringtheory = font.render("String Theory");
		textshader = FSShader("fonttest.frag");
		textshader->transform(glm::gtc::matrix_transform::scale(glm::mat4(1.0f), glm::vec3(0.5f,0.5f,1.0f)));
		hdrshader = FSShader("hdr.frag");
		hdrshader->program().use();
		glUniform1f(hdrshader->program().uniform("maxbright"), 1.f);
		glUniform1f(hdrshader->program().uniform("exposure"), 10.f);
		starfield = SimpleCLProgram("x=cos((f+1)*3295)*100+cos((f+1)*534684)*100; y=cos((f+1)*12484)*100+cos((f+1)*345645)*100; z=cos((f+1)*7484)*100+cos((f+1)*234534)*100;", 10000);
		starfieldshader = Program("starfield.frag", "starfield.geom", "starfield.vert");
		starfieldva = VArray();
		starfieldva->vertices(0,4, starfield->vertices());
		textshader->clear();
		
		auto sfcontrol = new shiva::controls::control<state>;
		sfcontrol->add(sflist);
		sfcontrol->add(new statelist);

		maintimer->listen(sfcontrol);
		sfcontrol->addevent(2);

		sl.add(sfcontrol);

		auto tc = new tcontrol<data<float>>;
		std::vector<time_point<float>> points;
		auto bi = std::back_inserter(points);
		bi = point(t(0),0.f);
		bi = point(t(4),1.f);
		bi = point(t(13),1.f);
		bi = point(t(14),0.f);
		add_points<float>(*tc, starlight, points, &cosine_ptime<float>);
		sl.add(tc);
		
		auto sfcontrolb = new shiva::controls::control<data<bool>>;
		sfcontrolb->add<bool>([&]{return true;}, starfieldpath);
		sfcontrolb->add<bool>([&]{return false;}, starfieldpath);

		sfcontrolb->addevent(2);
		maintimer->listen(sfcontrolb);
		sl.add(sfcontrolb);

		//	start
						//6 bars  fade-in, space
			maintimer->time(t(7));	
						//4 bars  add particles (text: shivaX001)
			maintimer->time(t(11));	
						//4 bars  (text: string theory)
			maintimer->time(t(15));	
						//8 bars  beatlines around comet
			maintimer->time(t(23));	
						//6 bars  comet explodes / crash
		//  end
	}

	~opening(){}
	virtual void display(){
		if(starfieldpath){
			fb->render([&]{
				glDepthMask(GL_TRUE);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				sl.display();
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glDepthMask(GL_FALSE);
				textshader->input("tex", shivatext);
				textshader->set("visibility", shivatextvisibility);
				textshader->display();
				textshader->input("tex", stringtheory);
				textshader->set("visibility", stringtheoryvisibility);
				textshader->display();
			
				starfieldshader->use();
				starfieldshader->set("red", 1.8f);
				starfieldshader->set("green", 0.8f);
				starfieldshader->set("blue", 0.5f);
				starfieldshader->set("visibility", starlight);
				glUniformMatrix4fv(starfieldshader->uniform("mat"), 1, false, glm::value_ptr(camera)); 
				starfieldshader->bindAttrib(0, "pos");
				glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
				starfieldva->draw(0, 10000, 1, GL_POINTS);
			});
		} else {
			fb->render([&]{
				glDepthMask(GL_TRUE);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				sl.display();
			});
		}
		hdrshader->input("tex", hdroutput);
		hdrshader->display();
	}

	virtual void update(){
		sl.update();
		
		starfield->limit(10000);
		starfield->update();
		
	}

	virtual bool stateUpdate(){
		bool ret = sl.stateUpdate();
		
		ret |= starfield->stateUpdate();
		ret = true;
		return ret;
	}

	virtual void reshape(int w, int h){
		sl.reshape(w,h);
		fb = FrameBuffer();
		hdroutput = *::hdrtexture;
		fb->colorbuffer(0, hdroutput);
	}
	
};