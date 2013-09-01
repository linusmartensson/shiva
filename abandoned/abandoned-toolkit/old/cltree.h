#pragma once
#include"clengine.h"
#include"stdafx.h"
#include"state.h"
#include<functional>

class cltreebase : public shiva::state{
protected:
	
	Instance<StatelessCLProgram> program;
	glm::vec3 trans, ptrans, pnorm, norm, ptang, tang;
	float pos, ppos;
	int lim, rplim, plim;
	glm::mat4 transform, ptransform;
	std::vector<cltreebase*> children, pchildren;
	
	void refresh(){
		program->refresh();
		for(auto i = pchildren.begin(); i!=pchildren.end(); ++i){
			(*i)->refresh();
		}
	}
	void _stateUpdate(){
		program->stateUpdate();
		pos = ppos;
		norm = pnorm;
		trans = ptrans;
		lim = rplim;
		children = pchildren;
		tang = ptang;
		transform = ptransform;
		for(auto i = pchildren.begin(); i!=pchildren.end(); ++i){
			(*i)->_stateUpdate();
		}
		
	}
public:
	cltreebase(StatelessCLProgram &p) : program(p), ptrans(0.0f), pnorm(0.0f), ptang(0.0f), ppos(0.0f), plim(0), rplim(0), ptransform(1.0f) {
		
	}
	VArray *varray(){
		return program->varray();
	}
	virtual ~cltreebase(){
		children.clear();
		for(auto i = pchildren.begin(); i != pchildren.end(); ++i){
			delete *i;
		}
	}
	void child(cltreebase *t){
		pchildren.push_back(t);
	}
	void matrix(glm::mat4 &m){
		ptransform = m;
	}
	glm::mat4& pmatrix(){
		return ptransform;
	}
	glm::mat4 matrix(){
		return transform;
	}
	
	void position(float l){
		l=l<0.0f?0.0f:(l>1.0f?1.0f:l);
		ppos = l;
	}
	void limit(float l){
		plim = int(l*program->maxLimit());
		for(auto i=children.begin();i!=children.end();++i){
			(*i)->limit(l);
		}
	}
	int& plimit(){
		return plim;
	}
	virtual void render(glm::mat4 m) = 0;
	void display(glm::mat4 m){
		if(trans.x != 0 || trans.y != 0 || trans.z != 0)
			m = glm::gtc::matrix_transform::translate(m, trans);
		
		m *= glm::mat4(glm::vec4(norm,0.f), glm::vec4(tang,0.f), glm::vec4(glm::normalize(glm::cross(norm,tang)),0.f), glm::vec4(0.f,0.f,0.f,1.f));
		
		m *= transform;
		render(m);
		for(auto i = children.begin(); i!=children.end(); ++i){
			(*i)->display(m);
		}
	}
	void display(){
		display(glm::mat4(1.0));
	}
	void update(cltreebase *parent, float limoff){
		if(parent != 0){
			int pos = int(ppos*parent->program->maxLimit());
			ptrans = parent->program->vertex(pos);
			pnorm = glm::normalize(parent->program->normal(pos));
			ptang = glm::normalize(parent->program->tangent(pos));
		} else {
			refresh();
			pnorm = glm::vec3(1.0f,0.0f,0.0f);
			ptang = glm::vec3(0.0f,1.0f,0.0f);
			ptrans = glm::vec3(0.0f,0.0f,0.0f);
			program->begin();
		}
		limoff += ppos;
		rplim = plim - static_cast<int>(limoff*program->maxLimit());
		rplim = rplim<0?0:(rplim>program->maxLimit()?program->maxLimit():rplim);
		program->update(false);
		for(auto i = pchildren.begin(); i!=pchildren.end(); ++i){
			(*i)->update(this, limoff);
		}
		if(parent==0){
			program->end();
		}

	}
	void update(){
		update(0, 0.0f);
	}
	bool stateUpdate(){
		_stateUpdate();
		return true;
	}
};

template<typename T>
class cltree : public cltreebase{
	T *renderer;
public:
	~cltree(){}
	cltree(StatelessCLProgram &p, T *renderer) : cltreebase(p), renderer(renderer) {}
	
	void render(glm::mat4 m){
		if(renderer != 0){
			renderer->matrix(m);
			renderer->vertexarray(*this->varray());
			renderer->limit(lim);
			renderer->display();
		}
	}
};

class ftree : public cltreebase{
	std::function<void(glm::mat4, VArray&, int)> f;
public:
	~ftree(){}

	template<typename T>
	ftree(StatelessCLProgram &p, T f) : cltreebase(p), f(std::function<void(glm::mat4, VArray&, int)>(f)) {}

	void render(glm::mat4 m){
		f(m,*varray(),lim);
	}
};

namespace shiva{
	namespace tree{
		template<typename T>
		cltree<T> *make_tree(StatelessCLProgram &p, T *renderer){
			return new cltree<T>(p, renderer);
		}
	}
};