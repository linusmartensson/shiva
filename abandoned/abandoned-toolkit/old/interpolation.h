#pragma once

#include"stdafx.h"

#include<functional>

namespace shiva{
	namespace interpolate{
		template<typename T>
		struct linear{
			T operator()(T a, T b, float c){
				return a*(1-c)+b*c;
			}
		};

		template<typename T> 
		struct cosine{
			linear<T> l;
			T operator()(T a, T b, float c){
 				return l(a, b, (1-cos(c*float(3.14159265)))/2);
			}
		};

		template<typename T, typename F, typename C>
		struct interpolate{
			F min, max;
			T vMin, vMax;
			std::function<T(T,T,float)> interpolator;
			std::function<C()> c;
			interpolate(T vMin, T vMax, F min, F max, std::function<T(T,T,float)> interpolator, std::function<C()> c) : vMin(vMin), vMax(vMax), c(c), min(min),max(max),interpolator(interpolator) {}
			T operator()(){
				return interpolator(vMin, vMax, float(((F)c())-min)/float(max-min));
			}
		};
		template<typename T, typename F>
		std::function<T()> cosine_ptime_interpolator(T vMin, T vMax, F min, F max){
			return interpolate<T,F,float>(vMin, vMax, min, max, cosine<T>(), shiva::core::physicsTime);
		}
		template<typename T, typename F>
		std::function<T()> linear_ptime_interpolator(T vMin, T vMax, F min, F max){
			return interpolate<T,F,float>(vMin, vMax, min, max, linear<T>(), shiva::core::physicsTime);
		}
		template<typename T, typename F>
		std::function<T()> cosine_time_interpolator(T vMin, T vMax, F min, F max){
			return interpolate<T,F,float>(vMin, vMax, min, max, cosine<T,F>(), shiva::core::renderTime);
		}
		template<typename T, typename F>
		std::function<T()> linear_time_interpolator(T vMin, T vMax, F min, F max){
			return interpolate<T,F,float>(vMin, vMax, min, max, linear<T,F>(), shiva::core::renderTime);
		}

		template<typename T, typename F>
		struct data_point{
			T vMin, vMax;
			F min, max;
			data_point(T vMin, T vMax, F min, F max) : vMin(vMin), vMax(vMax), min(min), max(max) {}
		};
		template<typename T, typename F>
		struct time_point{
			F time;
			T value;
			time_point(F time, T value) : time(time), value(value){}
			time_point() : time((F)0), value((T)0) {}
		};

		template<typename T, typename F>
		data_point<T, F> point(T vMin, T vMax, F min, F max){
			return data_point<T,F>(vMin, vMax, min, max);
		}
		template<typename T, typename F>
		time_point<T,F> point(F time, T value){
			return time_point<T,F>(time, value);
		}

		template<typename T, typename F>
		std::function<T()> cosine_ptime(data_point<T,F> p){
			return cosine_ptime_interpolator(p.vMin, p.vMax, p.min, p.max);
		}
		template<typename T, typename F>
		std::function<T()> cosine_time(data_point<T,F> p){
			return cosine_time_interpolator(p.vMin, p.vMax, p.min, p.max);
		}
		template<typename T, typename F>
		std::function<T()> linear_ptime(data_point<T,F> p){
			return linear_ptime_interpolator(p.vMin, p.vMax, p.min, p.max);
		}
		template<typename T, typename F>
		std::function<T()> linear_time(data_point<T,F> p){
			return linear_time_interpolator(p.vMin, p.vMax, p.min, p.max);
		}

		template<typename T, typename F, typename U>
		U& add_points(U &c, std::vector<data_point<T,F>> points, std::function<std::function<T()> (data_point<T,F>)> point_handler){
			for(auto i = points.begin(); i!= points.end(); ++i){
				c.add(point_handler(*i), i->min, i->max);
			}
			return c;
		}
		template<typename T, typename F, typename U>
		U& add_points(U &c, std::vector<time_point<T,F>> points, std::function<std::function<T()> (data_point<T,F>)> point_handler){
			if(points.size() < 2) return c;	// Nothing to interpolate between.
			auto last = points.begin();
			for(auto i = points.begin(), ++i; i!=points.end(); ++i, ++last){
				c.add(point_handler(point(last->value, i->value, last->time, i->time)), last->time, i->time);
			}
			return c;
		}template<typename T, typename F, typename U, typename J>
		U& add_points(U &c, J &link, std::vector<time_point<T,F>> points, std::function<std::function<T()> (data_point<T,F>)> point_handler){
			if(points.size() < 2) return c;	// Nothing to interpolate between.
			auto last = points.begin();
			for(auto i = ++points.begin(); i!=points.end(); ++i, ++last){
				c.add(point_handler(point(last->value, i->value, last->time, i->time)), link, last->time, i->time);
			}
			return c;
		}
	}
}