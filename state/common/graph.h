#pragma once 

#include"stdafx.h"
#include<map>
#include<string>
#include<algorithm>
#include"state.h"

class node{
	bool touched;
protected:
	std::vector<node*> children; 
public:
	node() : touched(false) {}
	virtual ~node() {}
	void traverse(std::function<void (node&)> &f){
		if(touched) return;
		touched = true;
		f(*this);
		std::for_each(children.begin(),children.end(), [&](node *n){
			n->traverse(f);
		});
	}
	void cleanup(){
		if(!touched) return;
		touched = false;
		std::for_each(children.begin(),children.end(), [](node *n){
			n->cleanup();
		});
	}
};

class graph {
	node *_root;

public:

	graph() {}
	~graph() {}

	node* root(){
		return _root;
	}

	void reroot(){
		_root = new node();
	}
	
	void traverse(std::function<void (node&)> &f){
		_root->traverse(f);
		_root->cleanup();
	}
};
