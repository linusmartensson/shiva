#pragma once

#include"stdafx.h"
#include"interpolation.h"
#include"state.h"
#include"shiva.h"
#include"controllers.h"
#include"data.h"
#include<sstream>

namespace shiva{
	namespace controllable{
		enum source{
			wheel,
			button,
			key,
			mousex,
			mousey
		};

		template<typename T>
		struct controldata{
			
			int pos, neg;
			source src;
			bool continuous;
			T &target;
			std::function<T(T, int)> f;
			bool pOn, nOn;

			controldata(T &target, std::function<T(T, int)> f, source src = wheel, int pos = 0, int neg = 0, bool continuous = false) : target(target), f(f), src(src), pos(pos), neg(neg), continuous(continuous), pOn(false), nOn(false){
			}
		};

		template<typename T>
		class control : public shiva::behaviour{
		public:

			
			virtual void run(eventmap &received, state *instance, boost::any &data) const { 
				if(received.count("winput") && d.src==wheel){
					if(up) target=f(target, 1);
					else target=f(target,-1);
				}
				if(received.count("binput") && d.src==button){
					if(btn==pos) pOn = state;
					if(btn==neg) nOn = state;

					if(!continuous){
						if(btn==pos && state) target=f(target,1);
						if(btn==neg && state) target=f(target,-1);
					}
				}
				if(received.count("minput")){
					if(src==mousex) target=f(target,x);
					if(src==mousey) target=f(target,y);
				}
				if(received.count("kinput") && d.src==key){
					if(k==pos) pOn = state;
					if(k==neg) nOn = state;

					if(!continuous){
						if(k==pos && state) target=f(target,1);
						if(k==neg && state) target=f(target,-1);
					}
				}
				if(continuous){
					if(pOn) target=f(target,1);
					if(nOn) target=f(target,-1);
				}
			}
			
			virtual void init(state *instance, boost::any &data) const {
				if(data.empty()){
					data = controldata<T>();
				}
			}
			virtual void render(childvec children, boost::any &data) const {}
			virtual void uninit(state *instance, boost::any &data) const {}
		};

		template<typename T>
		class interpolationbuilder : public shiva::state{

			std::vector<shiva::interpolate::time_point<T, DWORD>> points;
			int savekey, reloadkey, nextkey, prevkey;
			bool reload;
			std::string file;
			T &target;

			shiva::controls::tcontrol<shiva::data<T>, DWORD> interpolator;

			void load(){
				points.clear();
				std::string s = loadFile(file, true);
				std::istringstream iss(s);

				while(iss.good()){
					T val;
					DWORD time;
					iss>>val>>time;
					if(iss.good())
						points.push_back(shiva::interpolate::point(time, val));
				}
				interpolator.clear();
				shiva::interpolate::add_points<T, DWORD>(interpolator, target, points, shiva::interpolate::cosine_ptime<T, DWORD>);
			}

			void save(){
				std::ostringstream oss;
				for(auto i=points.begin(); i!=points.end();++i){
					auto p = *i;
					oss<<p.value<<" "<<p.time<<std::endl;
				}
				std::string s = oss.str();
				std::ofstream off(std::string("resources/")+file, std::ios_base::out);
				off<<s;

				interpolator.clear();
				shiva::interpolate::add_points<T, DWORD>(interpolator, target, points, shiva::interpolate::cosine_ptime<T, DWORD>);
			}
			int nextpoint(){
				DWORD t = DWORD(shiva::core::physicsTime());
				if(points.size() == 0) return 0;
				for(int i=0;i<(int)points.size();++i){
					if(points[i].time > t) return i;
				}
				return points.size(); // After last point.
			}
			int prevpoint(){
				DWORD t = DWORD(shiva::core::physicsTime());
				if(points.size() == 0) return 0;
				for(int i=(int)points.size()-1;i>=0;--i){
					if(points[i].time < t) return i;
				}
				return -1;
			}
			void addpoint(){
				DWORD t = DWORD(shiva::core::physicsTime());
				int i = nextpoint();
				if(i == points.size()){
					points.push_back(shiva::interpolate::point(t, target));
				} else {
					points.insert(points.begin()+i, shiva::interpolate::point(t, target));
				}
				save();
			}
			void atnextpoint(){
				int i = nextpoint();
				if(i == points.size()){
					points.push_back(shiva::interpolate::point(shiva::core::length(), target));
				} else {
					points[i].value = target;
				}
				save();
			}
			void atprevpoint(){
				int i = prevpoint();
				if(i == -1){
					shiva::interpolate::time_point<T,DWORD> t = shiva::interpolate::point(DWORD(0), target);
					points.insert(points.begin(), t);
				} else {
					points[i].value = target;
				}
				save();
			}
		public:

			interpolationbuilder(std::string file, T &target, int savekey=GLFW_KEY_ENTER, int reloadkey=GLFW_KEY_F5, int nextkey=GLFW_KEY_F12, int prevkey=GLFW_KEY_F11) : target(target), reloadkey(reloadkey), savekey(savekey), nextkey(nextkey), prevkey(prevkey), file(file), reload(false){
				load();
			}

			//Handles keyboard input.
			virtual void kinput(int key, bool state){
				if(key == savekey && !state && shiva::core::pause()){
					addpoint();
				}
				if(key == nextkey && !state && shiva::core::pause()){
					atnextpoint();
				}
				if(key == prevkey && !state && shiva::core::pause()){
					atprevpoint();
				}
				if(key == reloadkey && !state && shiva::core::pause()){
					load();
					reload = true;
				}
			}

			//Handles auxiliary thread updates. Multi-threaded environment.
			//Update runs on a fixed time-step loop incremented by TICK_TIME/call. In a paused environment
			//this property can be used to manage real-time interactions.
			virtual void update(){
				if(shiva::core::pause()) return;
				interpolator.update();
			}

			//Handles updates in the render state. Single-threaded environment.
			//Return true to indicate that a rendering state update has occurred and the screen needs to be updated.
			//Return false to indicate that the currently rendered frame is acceptable and does not need to be re-rendered.
			virtual bool stateUpdate(){
				if(shiva::core::pause() && !reload) return false;
				return interpolator.stateUpdate();
			}


		};

		template<typename T>
		void make_control(
			shiva::statelist &sl, 
			std::string file, 
			T &target, 
			std::function<T(T, int)> f, 
			typename source src = wheel, 
			int pos = 0, 
			int neg = 0, 
			bool continuous = false,
			int savekey=GLFW_KEY_ENTER, 
			int reloadkey=GLFW_KEY_F5, 
			int nextkey=GLFW_KEY_F12, 
			int prevkey=GLFW_KEY_F11){
				sl.add(new control<T>(target, f, src, pos, neg, continuous));
				sl.add(new interpolationbuilder<T>(file, target, savekey, reloadkey, nextkey, prevkey));
				
		}
	}
}