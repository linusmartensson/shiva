#pragma once
#include"state.h"
#include"shiva.h"
#include"ui/list.h"
#include"ui/field.h"
#include<functional>
/*
class projectstart : public shiva::state {
	
	enum states{
		loadstate,
		createstate,
		menustate
	};
	
	shiva::state *current;
	states state;
	
	std::function<void(std::string)> start;

	std::vector<std::string> items;
	void load(){
		state = loadstate;

		items.clear();
		DIR *ds = opendir("resources/projects");
		dirent *d;
		while(d = readdir(ds)){
			if(d->d_type != DT_DIR) continue;
			if(d->d_name[0] == '.') continue;
			items.push_back(std::string(&d->d_name[0], d->d_namlen));
		}
		closedir(ds);

		del.push_back(current);
		list *l;
		add.push_back(current = l = new list());
		l->setup(
			[&]{return items.size();},
			[&](int i){return items[i];},
			[&](int i){
				start("resources/projects"+items[i]);
			}
		);
		current->reshape(shiva::core::width(), shiva::core::height());
	}
	
	void startproject(std::string s){
		
		items.clear();
		DIR *ds = opendir("resources/projects");
		dirent *d;
		while(d = readdir(ds)){
			if(d->d_type != DT_DIR) continue;
			if(d->d_name[0] == '.') continue;
			items.push_back(std::string(&d->d_name[0], d->d_namlen));
		}
		closedir(ds);

		if(std::find(items.begin(),items.end(),s) != items.end()){
			Log::errlog()<<"Project already exists!"<<std::endl;
			return;
		}

		if(unsigned int(std::count_if(s.begin(),s.end(), [](char c){return isalnum(c)>0?1:0;}))<s.size()){
			Log::errlog()<<"Project name must be alphanumeric!"<<std::endl;
			return;
		}

		s = "resources/projects/"+s;
		mkdir(s.c_str());

		
		start(s);
	}

	void create(){
		state = createstate;
		del.push_back(current);
		field *f;
		add.push_back(current = f = new field());
		f->setup(
			[&](std::string s){
				startproject(s);
			},
			32
		);
		current->reshape(shiva::core::width(), shiva::core::height());
	}
	
	std::vector<std::pair<std::string, std::function<void()>>> entries;
	void menu(){
		state = menustate;
		if(current){del.push_back(current);}
		
		list *l;
		add.push_back(current = l = new list());
		l->setup(
			[&]{return entries.size();},
			[&](int i){return entries[i].first;},
			[&](int i){entries[i].second();}
		);
		current->reshape(shiva::core::width(), shiva::core::height());
	}	

public:

	projectstart() :current(0) {
	
		entries.push_back(std::make_pair("Create new project", [&]{
			create();
		}));
		entries.push_back(std::make_pair("Load project", [&]{
			load();
		}));
		entries.push_back(std::make_pair("Exit", []{
			shiva::core::stop();
		}));
		menu();
	}

	template<typename T>
	void setup(T start){
		this->start = start;
	}

	void kinput(int k, bool s){
		if(k == GLFW_KEY_ESC && s){
			if(state == menustate){
				shiva::core::stop();
			} else {
				menu();
			}
		}
		shiva::state::kinput(k,s);
	}

};*/