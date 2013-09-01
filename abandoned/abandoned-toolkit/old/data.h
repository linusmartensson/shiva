#pragma once
#include<functional>

namespace shiva{ 
	template<typename Data>
	class data : public state {
		Data m, pm, &om;
		std::function<Data ()> f;
	public:

		data(std::function<Data ()> f, const Data &pm = Data()) : m(pm), pm(pm), om(m), f(f) {}
		data(std::function<Data ()> f, Data &link, const Data &pm = Data()) : m(pm), pm(pm), om(link), f(f) {}

		Data get() const {
			return m;
		}
			
		virtual void update() {pm = f();}

		virtual bool stateUpdate(){
			om = pm;
			m = om;
			return true;
		}
		virtual void start(state *laststate){
			update();
			stateUpdate();
		}
	};
	template<typename Data>
	class rdata : public state {
		Data pm, &om;
		std::function<Data ()> f;
	public:

		rdata(std::function<Data ()> f, const Data &orig = Data()) : pm(orig), om(pm), f(f) {}
		rdata(std::function<Data ()> f, Data &link, const Data &pm = Data()) : pm(pm), om(link), f(f) {}

		Data get() const {
			return om;
		}
			
		virtual void display() {om = f();}

		virtual bool stateUpdate(){
			return true;
		}
		virtual void start(state *laststate){
			display();
			stateUpdate();
		}
	};
	template<typename Data>
	class pdata : public state {
		Data pm, &om;
		std::function<Data ()> f;
	public:

		pdata(std::function<Data ()> f, const Data &orig = Data()) : pm(orig), om(pm), f(f) {}
		pdata(std::function<Data ()> f, Data &link, const Data &pm = Data()) : pm(pm), om(link), f(f) {}

		Data get() const {
			return om;
		}
			
		virtual void update() {om = f();}

		virtual bool stateUpdate(){
			return true;
		}
		virtual void start(state *laststate){
			update();
			stateUpdate();
		}
	};

	template<typename DD, typename Data>
	DD* make_data(std::function<Data ()> c){
		return new DD(c,c());
	}
	template<typename Data, typename DD>
	DD* make_data(std::function<Data ()> c, Data &link){
		return new DD(c, link, c());
	}
	template<typename Data, typename DD, typename T>
	DD* make_data(T c){
		return new DD(std::function<Data ()>(c),std::function<Data ()>(c)());
	}
	template<typename DD, typename Data, typename T>
	DD* make_data(T c, Data &link){
		return new DD(std::function<Data ()>(c), link, std::function<Data ()>(c)());
	}
}