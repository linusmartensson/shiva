#include"stdafx.h"
#include"state.h"
#include"shiva.h"

namespace shiva{
	void state::reg(std::string action){
		boost::upgrade_lock<boost::shared_mutex> lock(mutex);
		if(registered.count(action) == 0){
			boost::upgrade_to_unique_lock<boost::shared_mutex> ulock(lock);
			registered[action]=1;
		}
	}
	void state::unreg(std::string action){
		boost::upgrade_lock<boost::shared_mutex> lock(mutex);
		if(registered.count(action)){
			boost::upgrade_to_unique_lock<boost::shared_mutex> ulock(lock);
			registered.erase(action);
		}
	}
	state* state::create(std::string name, std::string behaviour, state* parent, boost::any d) {
		auto s = new state();
		s->init(name, behaviour::fetch(behaviour), d);
		s->parent = parent;
		if(parent!=0){
			boost::unique_lock<boost::shared_mutex> plock(parent->pmutex);
			parent->childvec.push_back(s);
		}
		return s;
	}
	void state::run(){
		eventmap recv;
		{
			boost::upgrade_lock<boost::shared_mutex> lock(mutex);
			if(!initialized) return;
			recv = received;
			if(received.size() > 0){
				boost::upgrade_to_unique_lock<boost::shared_mutex> ulock(lock);
				received.clear();
			}
		}
		br->run(recv, this, data);
	}
	void state::render(){if(initialized) br->render(children(), data);}
	void state::touch(std::string action, const boost::any &evt){
		bool added = false;
		{
			boost::upgrade_lock<boost::shared_mutex> lock(mutex);
			if(registered.count(action) && received.count(action)==0){
				added = true;
				boost::upgrade_to_unique_lock<boost::shared_mutex> ulock(lock);
				received[action] = evt;
			}
		}
		if(added){
			shiva::core::on(name+":handled:"+action, evt);
		}
		auto s = children();
		std::for_each(s.begin(),s.end(),[&](state *c){c->touch(action, evt);});
	}
}