#pragma once 

#include"stdafx.h"
#include<map>
#include<set>
#include<string>
#include<algorithm>
#include"state.h"

template<typename node = void*, typename edge = void*>
class graph {
	std::set<node*> nodelist;
	std::map<node*,std::map<node*, edge*>> edgelist;
public:
	graph(){}
	~graph(){std::for_each(nodelist.begin(),nodelist.end(),[&](node *n){delete n;});}

	node* put(){
		auto t = new node;
		nodelist.insert(t);
		return t;
	}
	node* put(size_t count){
		auto t = new node[count];
		for(size_t i=0;i<count;++i){
			nodelist.insert(t);
		}
		return t;
	}
	edge* link(node *a, node *b){
		auto t = new edge;
		edgelist[a][b] = t;
		return t;
	}
	void split(node *a, node *b){
		delete edgelist[a][b];
		edgelist[a].erase(b);
	}
	const std::set<node*>& nodes() const {
		return nodelist;
	}
	const std::map<node*, std::map<node*, edge*>>& edges() const {
		return edgelist;
	}
	const std::map<node*, edge*>& edges(node* a) const {
		return edgelist[a];
	}
};
