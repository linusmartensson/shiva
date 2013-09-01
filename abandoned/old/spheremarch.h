#pragma once

#include"stdafx.h"
#include"state.h"
#include"shaders.h"
#include"input.h"
#include"framebuffer.h"
#include"camera.h"
#include"controllers.h"

using namespace shiva;


class spheremarch : public state {
	Instance<FSShader> fss;
	Instance<Texture> tn;
	std::vector<Instance<Texture>> tvec, d;
	std::vector<Instance<FrameBuffer>> f;
	hdrstate *hs;
	
	float xd, yd;
	glm::vec3 trans;
	glm::mat3 cam;

public:

	spheremarch(hdrstate *hs) : hs(hs), xd(0.0f), yd(0.0f), trans(0.0f), cam(0.0f) {
		onresources *ok = new onresources;
		ok->add([&]{fss->rebuild(Shader::version330() + Shader::noiselibs() + "statedata/spheremarch/spheremarch.frag");}, false);
		sl.add(ok);

		fss = FSShader(Shader::version330() + Shader::noiselibs() + "statedata/spheremarch/spheremarch.frag");
		
		auto c = new camera("spheremarch", trans, xd, yd, 'W', 'S', 'A', 'D');
		sl.add(c);
		
		auto camera = new controls::tcontrol<shiva::data<glm::mat3>>();
		sl.add(camera);
		camera->add<glm::mat3>([&]{
			return glm::mat3(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 50000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), yd, glm::vec3(1,0,0))*glm::rotate(glm::quat(), xd, glm::vec3(0,1,0)))
				);
		}, cam, t(1), t(500));
	}
	void reshape(int w, int h){
		
		tn = Texture(1, GL_R16F, GL_TEXTURE_2D, 1,1);
		int i = 0;
		while(w/(int)pow(2.0, (int)i+1) > 1 && h/(int)pow(2.0, (int)i+1) > 1){
			tvec.push_back(Instance<Texture>());
			d.push_back(Instance<Texture>());
			f.push_back(Instance<FrameBuffer>());
			tvec[i] = Texture(1, GL_R16F, GL_TEXTURE_2D, w/(int)pow(2.0,(int)i+1), h/(int)pow(2.0,(int)i+1));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			d[i] = Texture(1, GL_DEPTH_COMPONENT24, GL_TEXTURE_2D, w/(int)pow(2.0,(int)i+1), h/(int)pow(2.0,(int)i+1));
			f[i] = FrameBuffer();
			f[i]->colorbuffer(0, tvec[i]);
			f[i]->depthbuffer(d[i]);
			++i;
		}
	}
	void display(){
		sl.display();
		hs->acc(0,0.9f,0.3);
		hs->accblur(1,1);
		hs->bloomblur(1,1);
		hs->mix(1,0.05,0.05);
		hs->toneMapping(10,0.2);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		for(int i=f.size()-1;i>=0;--i){
			f[i]->render([&]{
				fss->set("depthout", 1.0f);
				if(i == f.size()-1){
					fss->input("depthin", tn);
					fss->set("usedepthin", 0.0f);
				} else {
					fss->input("depthin", tvec[i+1]);
					fss->set("usedepthin", 1.0f);
				}
				fss->set("camerarot", cam);
				fss->set("camerapos", trans);
				fss->set("resolution", glm::vec2(shiva::core::width(), shiva::core::height()));
				fss->set("texresolution", glm::vec2(shiva::core::width()/pow(2.0,i+1), shiva::core::height()/pow(2.0,i+1)));
				fss->set("t", shiva::core::physicsTime()*0.001f);
				fss->display();
			});
		}
		fss->set("depthout", 0.0f);
		fss->input("depthin", tvec[0]);
		fss->set("usedepthin", 1.0f);
		fss->set("camerarot", cam);
		fss->set("camerapos", trans);
		fss->set("resolution", glm::vec2(shiva::core::width(), shiva::core::height()));
		fss->set("texresolution", glm::vec2(shiva::core::width(), shiva::core::height()));
		fss->set("t", shiva::core::physicsTime()*0.001f);
		fss->display();
		
	}

};