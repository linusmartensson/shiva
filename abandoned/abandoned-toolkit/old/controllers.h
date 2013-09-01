#pragma once
#include"stdafx.h"
#include"state.h"
#include"shiva.h"
#include<vector>
#include<functional>
#include<iterator>
namespace shiva{
	namespace controls{

		/*Note: Any state changing functions, e.g. time(), next(), set(), should only be called during single thread contexts (stateUpdate(), constructor/destructor)*/

		template<typename T, typename F=float>
		struct ds{
			static unsigned int id_counter;
			unsigned int id;
			T* target;
			F low;
			F high;
			ds(T* target, F low, F high) : id(id_counter++), target(target), low(low), high(high) {}
			ds() : id(id_counter++), target(0), low(0), high(0) {}
			bool operator <(const ds &rhs){
				return id<rhs.id;
			}
		};
		template<typename T, typename F=float>
		class tcontrol : public shiva::state{
			
			std::vector<ds<T,F>> data;
			std::vector<ds<T,F>> currentset;
		public:
			void clear(){
				data.clear();
				currentset.clear();
			}
			void reshape(int w, int h){
				for(auto i =data.begin(); i!=data.end(); ++i){
					i->target->reshape(w, h);
				}
			}
			bool stateUpdate(){
				bool ret = false;
				F ct = (F)core::time();
				std::vector<ds<T,F>> input, diff;
				for(auto i=data.begin();i!=data.end();++i){
					if(i->low <= ct && i->high > ct) input.push_back(*i);
				}
				std::set_difference(
					input.begin(),
					input.end(),
					currentset.begin(),
					currentset.end(), std::back_inserter(diff)
					);
				for(auto i=diff.begin();i!=diff.end();++i){
					i->target->update();
					ret = true;
				}
				currentset = input;
				for(auto i=currentset.begin(); i!=currentset.end(); ++i){
					ret |= i->target->stateUpdate();
				}
				return ret;
			}
			void update(){
				for(auto i=currentset.begin(); i!=currentset.end(); ++i){
					i->target->update();
				}
			}
			void display(){
				for(auto i=currentset.begin(); i!=currentset.end(); ++i){
					i->target->display();
				}
			}
			
			virtual void winput(bool up){
				for(auto i=currentset.begin(); i!=currentset.end(); ++i){
					i->target->winput(up);
				}
			}


			virtual void binput(int btn, bool state){
				for(auto i=currentset.begin(); i!=currentset.end(); ++i){
					i->target->binput(btn, state);
				}
			}


			virtual void minput(int x, int y){
				for(auto i=currentset.begin(); i!=currentset.end(); ++i){
					i->target->minput(x, y);
				}
			}


			virtual void kinput(int key, bool state){
				for(auto i=currentset.begin(); i!=currentset.end(); ++i){
					i->target->kinput(key, state);
				}
			}

			template<typename Q>
			tcontrol<T,F>& add(std::function<Q()> j, F low, F high){
				data.push_back(ds<T,F>(shiva::make_data<T>(j), low, high));
				std::sort(data.begin(), data.end());
				return *this;
			}
			template<typename Q, typename U>
			tcontrol<T,F>& add(std::function<Q()> j, U &link, F low, F high){
				data.push_back(ds<T,F>(shiva::make_data<T>(j, link), low, high));
				return *this;
			}
			tcontrol<T,F>& add(T* t, F low, F high){
				data.push_back(ds<T,F>(t, low, high));
				return *this;
			}
		};
		template<typename T, typename F>
		unsigned int ds<T,F>::id_counter = 0;

	}
}