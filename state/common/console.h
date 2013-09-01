#pragma once

#include"shiva.h"
#include"state.h"
#include"texture.h"
using namespace shiva;


class console : public behaviour {

	struct data{
		std::vector<std::string> lines;
		std::vector<Texture*> tlines;
		int topline;
		glm::ivec2 marker;
		glm::ivec2 displaysize;
		bool active;
		bool enabled;
		std::string file;
		int updated;
		
		void scrollto(int pos) const {

		}
		void insertline(int pos, std::string line) const {

		}
		void updateline(int pos) const {

		}
	};


	void run(eventmap &received, state *instance, boost::any &data) const {
		auto &d = boost::any_cast<console::data&>(data);
		if(!d.enabled) return;

		if(!d.active) return;
	};
	void render(childvec children, boost::any &data) const {
		auto &d = boost::any_cast<console::data&>(data);
		if(!d.enabled) return;
		if(!d.active) return;
		//Render each line
	};
	void init(state *instance, boost::any &data) const {
			
	};
	void uninit(state *instance, boost::any &data) const {
			
	};

};
