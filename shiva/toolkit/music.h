#pragma once
#include"state.h"
#include<string>
#include"shiva.h"

class music {
	FMOD::Sound *sound;
	FMOD::Channel *channel;
	FMOD::System *system;

	bool paused;

public:
	enum SPECTRUM_CHANNEL{
		SPECTRUM_LEFT_CHANNEL = 0,
		SPECTRUM_RIGHT_CHANNEL = 1
	};
	void time(float t){
		channel->setPosition((unsigned int)(t*1000), FMOD_TIMEUNIT_MS);
	}
	float time(){
		unsigned int pos;
		channel->getPosition(&pos, FMOD_TIMEUNIT_MS);
		return pos/1000.f;
	}
	std::vector<float> spectrum(SPECTRUM_CHANNEL sc = SPECTRUM_LEFT_CHANNEL){
		std::vector<float> v(1024);
		system->getSpectrum(&(v[0]), 1024, sc, FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS);
		return v;
	}
	std::vector<float> waveform(SPECTRUM_CHANNEL sc = SPECTRUM_LEFT_CHANNEL){
		std::vector<float> v(1024);
		system->getWaveData(&(v[0]), 1024, sc);
		return v;
	}
	bool pause() const {return paused;}
	void pause(bool p){
		channel->setPaused(p);
		paused = p;
	}
	music(std::string f) {
		paused = true;
		Log::FMODErrors(FMOD::System_Create(&system), "FMOD::System_Create(&system)");
		system->setDSPBufferSize(128, 10);
		Log::FMODErrors(system->init(1, FMOD_INIT_NORMAL, 0), "system->init(1, FMOD_INIT_NORMAL, 0)");
		std::string soundfile = "resources/"+f;
		Log::info()<<"audio file: "<<soundfile<<std::endl;
		Log::FMODErrors(system->createStream(soundfile.c_str(), FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &sound), "system->createStream(soundfile.c_str(), FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &sound)");
	
		Log::FMODErrors(system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel), "system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel)");
		channel->setPaused(true);
	}
	~music(){
		Log::info()<<"destroying audio..."<<std::endl;
		Log::FMODErrors(sound->release(), "sound->release()");
		Log::FMODErrors(system->close(), "system->close()");
		Log::FMODErrors(system->release(), "system->release()");
	}
};

class musicplayer : public shiva::behaviour {
struct musicplayerdata{music *m;};
public:
	void init(shiva::state *s, boost::any &data) const {
		auto str = boost::any_cast<std::string&>(data);
		data = musicplayerdata();
		boost::any_cast<musicplayerdata&>(data).m = new music(str);
		s->reg(s->get()+"/play");
		s->reg(s->get()+"/goto");
		s->reg("frame");
	}
	void render(shiva::childvec children, boost::any &data) const {}
	void run(shiva::eventmap &received, shiva::state *s, boost::any &data) const {
		if(received.count("frame")){
			shiva::core::on(s->get()+"/time", boost::any_cast<musicplayerdata&>(data).m->time());
			shiva::core::on(s->get()+"/wform", boost::any_cast<musicplayerdata&>(data).m->waveform());
			shiva::core::on(s->get()+"/spectrum", boost::any_cast<musicplayerdata&>(data).m->spectrum());
		}
		if(received.count(s->get()+"/play")){
			 boost::any_cast<musicplayerdata&>(data).m->pause(!boost::any_cast<musicplayerdata&>(data).m->pause());
		}
		if(received.count(s->get()+"/goto")){
			boost::any_cast<musicplayerdata&>(data).m->time(boost::any_cast<float>(received[s->get()+"/goto"]));
		}
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {
		auto mpl = boost::any_cast<musicplayerdata&>(data);
		delete mpl.m;
	}
};
behaviour_add(musicplayer);
