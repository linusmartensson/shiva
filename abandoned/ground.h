#pragma once
#include"stdafx.h"
#include"clengine.h"
#include"varray.h"
#include"shaders.h"
#include"framebuffer.h"
#include"cltree.h"

class ground : public shiva::state {
	static const unsigned int particleCount = 1000;
	struct clear{
		void display(){
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	};
	struct renderer{
		ground *l;
		Instance<VArray> va;
		int lim;
		glm::mat4 mat;
		renderer() : lim(1) {}
		void matrix(glm::mat4 m){mat = m;}
		void vertexarray(VArray &v){va = v;}
		void limit(int c){lim = c;}
		void display(){
			/*glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			l->p.use();
			l->p.bindAttrib(0, "in_Position");
			glUniformMatrix4fv(l->p.uniform("m"), 1, GL_FALSE, glm::value_ptr(mat));
			va.draw(0,lim, 1, GL_TRIANGLE_STRIP);
			*/
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			l->p2.use();
			glUniform1f(l->p2.uniform("red"), 0.5f);
			glUniform1f(l->p2.uniform("green"), 0.3f);
			glUniform1f(l->p2.uniform("blue"), 0.07f);
			l->p2.bindAttrib(0, "in_Position");
			glUniformMatrix4fv(l->p2.uniform("m"), 1, GL_FALSE, glm::value_ptr(mat));
			va->draw(0,lim, 1, GL_POINTS);
			glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
	};
	struct renderer2{
		ground *l;
		Instance<VArray> va;
		int lim;
		glm::mat4 mat;
		renderer2() : lim(1) {}
		void matrix(glm::mat4 m){mat = m;}
		void vertexarray(VArray &v){va = v;}
		void limit(int c){lim = c;}
		void display(){
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			
			l->p2.use();
			glUniform1f(l->p2.uniform("red"), 0.1f);
			glUniform1f(l->p2.uniform("green"), 0.3f);
			glUniform1f(l->p2.uniform("blue"), 0.05f);

			l->p2.bindAttrib(0, "in_Position");
			glUniformMatrix4fv(l->p2.uniform("m"), 1, GL_FALSE, glm::value_ptr(mat));
			va->draw(0,lim, 1, GL_POINTS);
			glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
	};
	Program p, p2;
	FrameBuffer fb;
	Instance<FSShader> fss;
	Instance<Texture> t, t2;
	renderer r;
	renderer2 r2;
	cltree<renderer> ru;
	std::vector<cltreebase*> children;
public:	

	ground(int argc, char **argv) : 
		p2("lines_h/minimal2.frag","lines_h/minimal2.geom","lines_h/particles2.vert"),
		ru(StatelessCLProgram("float fpi = 3.14159*f*40.0;x = sin(fpi*0.5)*fpi;y=-fpi*0.3;z=cos(fpi*0.5)*fpi;xt=0;yt=1;zt=0;", particleCount*2), &r) {
		using namespace shiva::tree;
		r.l = this;
		r2.l = this;
		auto s = StatelessCLProgram(std::string()+
			"float fpi = 3.14159*f*20.0;"+
			"x=sin(fpi*3.4)*sin(fpi*7.4); \
			y=sin(fpi*6.7); \
			z=f*20.0;"+
			"float xx = sqrt(x*x+y*y);x*=xx*2;y*=xx*2;"+
			"float x2=cos(fpi*3.4)*cos(fpi*7.4+f*sin(acctime*1.2)*0.10f)*4.0;  \
			float y2=cos(fpi*6.7)*4.5;  \
			float z2=cos(fpi*1.9)*14.9;  \
			float4 ff = cross((float4)(x,y,z,0.0f),(float4)(x2,y2,z2,0.0f)); \
			xt=ff.x,yt=ff.y,zt=ff.z;", particleCount*2);
		auto t = StatelessCLProgram("float fpi = f*20.0f; x = sin(acctime*0.03)*cos(acctime*0.05)*f*0.1; y = fpi; z = cos(acctime*0.02)*f*0.1;xt=cos(fpi*320);yt=0;zt=sin(fpi*320);", particleCount/10);
		
		cltreebase *parent = 0;
		std::vector<cltreebase*> v;
		v.push_back(&ru);
		for(int i=1;i<400;++i){

			auto clt = make_tree(t, &r);
			int p = std::max(int(0),int(v.size()-1-rand()%100) );
			v[p]->child(clt);
			//ru.child(clt);
			children.push_back(clt);
			clt->position((rand()%500)/500.0f);
			if(p) clt->matrix(glm::gtc::matrix_transform::rotate(glm::mat4(1.0f), 60.f, glm::vec3(1.f,0.f,0.f)));
			v.push_back(clt);
		}

		for(int i=1;i<300;++i){
			auto clt = make_tree(s, &r2);
			v[rand()%v.size()]->child(clt);
			//ru.child(clt);
			children.push_back(clt);
			clt->position(0);
			v.push_back(clt);
		}
	}

	void display(){
		clear c;
		fb.render(&c, t,t2);
		fb.render(&ru,t,t2);
		fss->input("tex", t);
		fss->display();

	}
	bool stateUpdate(){
		ru.stateUpdate();
		return true;
	}
	void reshape(int w, int h){
		t = Texture(8, GL_RGB16F);
		t2 = Texture(8, GL_DEPTH_COMPONENT24);
		fss = FSShader("hdr.frag");
		fss->program().use();
		glUniform1f(fss->program().uniform("maxbright"), 2.0f);
		glUniform1f(fss->program().uniform("exposure"), 1.0f);
	}
	void update(){
		ru.matrix(glm::gtc::matrix_transform::perspective(45.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::gtc::matrix_transform::lookAt(
					glm::vec3(
						cos(shiva::core::physicsTime()*0.0001)*200.0,
						100,
						sin(shiva::core::physicsTime()*0.0001)*200.0), 
					glm::vec3(0,0,0), glm::vec3(0,1,0))
				);
		ru.limit(abs(sin(shiva::core::physicsTime()*0.0001f*0.2f))*30.f);

		ru.update();
		
	}
	void start(state *laststate){
	}
};
