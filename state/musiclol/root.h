
#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"shiva.h"
#include<fmod.hpp>

struct rootdata{
	FMOD::System *s;
	FMOD::Channel *c;
	FMOD::Sound *o;
	std::function<FMOD_RESULT (void*, unsigned int datalen)> cb;
};
FMOD_RESULT F_CALLBACK soundcb(FMOD_SOUND *a, void *b, unsigned int datalen){
	FMOD::Sound &f = *(FMOD::Sound *)a;
	rootdata *d;
	f.getUserData((void**)&d);
	return d->cb(b,datalen);
}
class root : public shiva::behaviour {
	virtual void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		if(received.count("kinput")){
			if(shiva::core::down(GLFW_KEY_ESC)){
				shiva::core::stop();
			}
		}
	}
	virtual void render(shiva::childvec children, boost::any &data) const {}
	virtual void init(shiva::state *instance, boost::any &data) const {
		instance->reg("kinput");
		data = rootdata();
		auto &d = boost::any_cast<rootdata&>(data);
		FMOD::System_Create(&d.s);
		
		FMOD_CREATESOUNDEXINFO fs;
		memset(&fs, 0, sizeof(fs));
		
		fs.cbsize = sizeof(fs);
		
		d.s->setSoftwareFormat(48000,FMOD_SOUND_FORMAT_PCMFLOAT,0,0,FMOD_DSP_RESAMPLER_LINEAR);
		Log::FMODErrors(d.s->init(100, FMOD_INIT_NORMAL, 0), "init");
		float *dd = new float[48000];
		for(int i=0;i<48000;++i){
			dd[i] = sin(i*400*3.14159f*2/48000.f);
		}
		Log::FMODErrors(d.s->createSound("resources/statedata/dd/french_sexy_maid.mp3", 
			FMOD_SOFTWARE, &fs, &d.o), "createStream");
		Log::FMODErrors(d.s->playSound(FMOD_CHANNEL_FREE, d.o, false, &d.c), "play sound");
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(root);

