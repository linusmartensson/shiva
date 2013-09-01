#pragma once

template<class T>
class Instance{
	T *t;
public:
	~Instance(){
		if(t) delete t;
		t=0;
	}
	Instance(T &rhs) : t(new T(rhs)) {
	}
	Instance(Instance<T> &rhs) : t(0) {
		if(rhs.t!=0)
			t = new T(*(rhs.t));
	}
	Instance():t(0) {}
	T& operator=(T const &rhs){
		if(t) delete t;
		t = new T(rhs);
		return *t;
	}
	Instance<T>& operator=(Instance<T> const &rhs){
		if(&rhs == this) return *this;
		if(t) delete t;
		t = new T(*rhs.t);
		return *this;
	}
	T* operator ->(){
		return t;
	}
	T& operator *(){
		return *t;
	}
	operator Instance<T>& (){
		return *this;
	}
	operator T& (){
		assert(t!=0);
		return *t;
	}
	operator Instance<T>* (){
		return this;
	}
	operator T* (){
		return t;
	}
	void clear(){
		if(t) delete t;
		t=0;
	}
};