#pragma once
#include<vector>
#include<algorithm>
#include<boost/thread.hpp>
#include<boost/any.hpp>
#include<map>
#include<set>

namespace shiva{

	class state;

	struct state_exists{};

	typedef std::map<std::string, boost::any> eventmap;
	typedef const std::vector<shiva::state*>& childvec;

	//Flyweight manager
	class behaviour{

		//Not thread safe(!)
		static std::map<std::string, behaviour *>& behaviours(){
			static std::map<std::string, behaviour*> data;
			return data;
		}

		static boost::shared_mutex cmutex;
	public:
		virtual void run(eventmap &received, state *instance, boost::any &data) const = 0;
		virtual void render(childvec children, boost::any &data) const = 0;
		virtual void init(state *instance, boost::any &data) const = 0;
		virtual void uninit(state *instance, boost::any &data) const = 0;
		virtual ~behaviour(){}
		
		static behaviour* fetch(std::string s){
			boost::unique_lock<boost::shared_mutex> lock(cmutex);
			auto i = behaviours().find(s);
			if(i == behaviours().end()) return 0;
			return i->second;
		}

		static void add(std::string s, behaviour *b){
			boost::unique_lock<boost::shared_mutex> lock(cmutex);
			if(behaviours().count(s) != 0){
				std::ostringstream oss;
				oss<<s<<"#rename!"<<behaviours().size();
				behaviours()[oss.str()] = behaviours()[s];
				Log::errlog()<<"Warning! existing behaviour replaced in factory: "+s<<std::endl;
			}
			behaviours()[s] = b;
		}
		
		static behaviour* destroy(){
			boost::unique_lock<boost::shared_mutex> lock(cmutex);
			std::for_each(behaviours().begin(), behaviours().end(), [](const std::pair<std::string, behaviour*> &p){delete p.second;});
			behaviours().clear();
		}

	};

#define behaviour_add(name) \
	template<typename T> class _behaviour##name { public: _behaviour##name(){ shiva::behaviour::add(#name, new T()); }}; \
	_behaviour##name<name> _behaviour##name##_instance;

	class state{
		eventmap received;
		std::map<std::string, int> registered;
		boost::shared_mutex mutex, pmutex;
		boost::any data;
		behaviour *br;
		std::string name;
		bool initialized;
		std::vector<state*> childvec;
		state *parent;

		static boost::shared_mutex cmutex;
		static std::map<std::string, state*> statemap;
		
	private:
		state& operator=(const state& rhs);
		state() : initialized(false) {}
		~state(){}
		void init(std::string name, behaviour *behaviour, boost::any d){
			data = d;
			this->name = name;
			br = behaviour; 
			{
				boost::upgrade_lock<boost::shared_mutex> lock(cmutex);
				if(statemap.count(name)>0){
					throw state_exists();
				} else {
					boost::upgrade_to_unique_lock<boost::shared_mutex> ulock(lock);
					statemap[name] = this;
				}
			}
			br->init(this, data);
			{
				boost::unique_lock<boost::shared_mutex> ulock(mutex);
				initialized = true;
			}
		}
	public:
		
		std::string get() const { return name; }

		
		void touch(std::string action, const boost::any &evt);
		void render();
		void run();
		void reg(std::string);
		void unreg(std::string);
		std::vector<state*> children(){
			boost::shared_lock<boost::shared_mutex> plock(pmutex);
			std::vector<state*> cv = childvec;
			return cv;
		}
		static std::vector<state*> states(){
			std::vector<state*> s;
			boost::shared_lock<boost::shared_mutex> lock(cmutex);
			auto f = [&s](std::pair<std::string, state *> i){s.push_back(i.second);};
			std::for_each(statemap.begin(), statemap.end(), f);
			return s;
		}
		static state* get(std::string name) {
			boost::shared_lock<boost::shared_mutex> lock(cmutex);
			if(statemap.count(name)){
				return statemap[name];
			}
			return 0;
		}
		static state* create(std::string name, std::string behaviour, state* parent, boost::any d = boost::any());
		state* create(std::string name, std::string behaviour, boost::any d = boost::any()){
			return state::create(name, behaviour, this, d);
		}
		static void destroy(std::string name){
			boost::unique_lock<boost::shared_mutex> ulock(cmutex);
			auto i = statemap.find(name);
			if(i != statemap.end()){
				//TODO:: recursive delete. (lookit this leak)
				state *s = i->second;
				s->br->uninit(s, s->data);
				delete i->second;
				statemap.erase(i);
			}
		}
	};
}
