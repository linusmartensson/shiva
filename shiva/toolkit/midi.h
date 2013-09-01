#pragma once
#include<vector>
#include<fstream>
#include<sstream>
#include<iostream>
#include<algorithm>
class midi {

	int format, trackCount;
	float timediv;
	bool reltime;
	std::vector<std::pair<int, int>> tempoChanges;

	static int val(std::ifstream &ifs, int len, int &pos) {
		if(len == 0) return 0;
		int t = 0;
		for(int i=0;i<len;++i){
			pos++;
			unsigned char c;
			
			ifs>>c;
			t=(t<<8)|(c&0xFF);
		}
		return t;
	}
	static int vval(std::ifstream &ifs, int &pos){
		int value = 0;
		unsigned char c=0;
		ifs>>c;
		pos++;
		value = c;
		if(value & 0x80){
			value &= 0x7f;
			do{
				ifs>>c;
				pos++;
				value = (value<<7) + (c&0x7f);
			}while(c&0x80);
		}
		return value;
	}
	static std::string str(std::ifstream &ifs, int len, int &pos){
		if(len == 0) return "";
		std::ostringstream oss;
		for(int i=0;i<len;++i){
			pos++;
			unsigned char c;
			ifs>>c;
			oss<<c;
		}
		return oss.str();
	}
	struct track{
		int size;
		float trackpos;
		std::vector<std::pair<int, std::pair<float, float>>> notes;

		track(std::ifstream &ifs, bool reltime, float timediv, std::vector<std::pair<int, int>> &tempoChanges){
			//static header
			int i=0;
			std::string s = str(ifs, 4, i);
			size = val(ifs, 4, i);

			std::vector<std::pair<int, float>> on;
			
			//Defaults I care about.
			trackpos = 0.f;

			bool endOfTrack = false;
			int pos = 0;
			int status = 0x0;
			int dsum = 0;
			size_t tpos = 0;
			while(!endOfTrack){
				
				int delta = vval(ifs, pos);
				
				dsum += delta;

				int dv = delta;

				//Note: Will desync iff there is an SMPTPsomething offset on the tempo track.
				while(tempoChanges.size() > tpos+1 && dsum >= tempoChanges[tpos+1].first) {
					

					dv = dsum-tempoChanges[tpos+1].first;

					int p = delta-dv;

					if(reltime == 0) trackpos += p/timediv;
					else trackpos += p*((tempoChanges[tpos].second/1000000.f)/timediv);
					
					tpos++;
				}

				if(reltime == 0) trackpos += dv/timediv;
				else trackpos += dv*((tempoChanges[tpos].second/1000000.f)/timediv);
				

				int evt = val(ifs, 1, pos);
				int type = (evt&0xF0)>>4;
				int channel = (evt&0x0F);
				
				if(type != 0xF){
					//Standard MIDI event.

					int p1, p2;

					if(!(evt&0x80)){
						p1 = evt;
						evt = status;
						type = (evt&0xF0)>>4;
						channel = (evt&0x0F);
					} else {
						p1 = val(ifs, 1, pos);
						status = evt;
					}
					if(type != 0xC && type != 0xD)
						p2 = val(ifs, 1, pos);

					switch(type){
					case 0x8:
					case 0x9:
						if((type==0x9 && p2==0) || type==0x8){
							on.push_back(std::make_pair(p1, trackpos));
						} else {
							size_t i=0;
							for(;i<on.size();++i){
								if(on[i].first == p1){
									break;
								}
							}
							if(i<on.size()){
								notes.push_back(std::make_pair(on[i].first, std::make_pair(on[i].second, trackpos)));
								on.erase(on.begin()+i);
							}
						}
						break;
					case 0xA:
						break;
					case 0xB:
						break;
					case 0xC:
						break;
					case 0xD:
						break;
					case 0xE:
						break;
					}
				} else {
					if(channel == 0xF){
						int h,m,s,fr,sfr,frt;
						float fps;
						//MIDI meta event.
						int type = val(ifs, 1, pos);
						int len = vval(ifs, pos);
						switch(type){
						case 0x0:
							val(ifs,len, pos);
							break;
						case 0x1:
							str(ifs,len, pos);
							break;
						case 0x2:
							str(ifs,len, pos);
							break;
						case 0x3:
							str(ifs,len, pos);
							break;
						case 0x4:
							str(ifs,len, pos);
							break;
						case 0x5:
							str(ifs,len, pos);
							break;
						case 0x6:
							str(ifs,len, pos);
							break;
						case 0x7:
							str(ifs,len, pos);
							break;
						case 0x20:
							val(ifs,len, pos);
							break;
						case 0x2F:
							val(ifs,len, pos);
							endOfTrack = true;
							break;
						case 0x51:
							tempoChanges.push_back(std::make_pair(dsum, val(ifs, len, pos)));
							break;
						case 0x54:
							h = val(ifs, 1, pos);
							m = val(ifs, 1, pos);
							s = val(ifs, 1, pos);
							fr = val(ifs, 1, pos);
							sfr = val(ifs, 1 , pos);
							frt = (h&0xE0)>>5;
							h = h&0x1F;
							fps = frt==0?24:(frt==1?25:(frt==2?29.97f:30));
							trackpos += h*60*60+m*60+s+fr/fps+fr/(fps*100);
							
							break;
						case 0x58:
							val(ifs,len, pos);
							break;
						case 0x59:
							val(ifs,len, pos);
							break;
						case 0x7F:
							val(ifs,len, pos);
							break;
						default:
							val(ifs,len,pos);
						}
					} else if(channel == 0x0 || channel == 0x7){
						int len = vval(ifs, pos);
						val(ifs, len, pos);
					}
				}
			}
			std::sort(notes.begin(), notes.end(), [](std::pair<int, std::pair<float,float>> a, std::pair<int, std::pair<float,float>> b){
				return a.second.first<b.second.first;
			});
		}
	};

	
public:
	std::vector<midi::track> tracks;
	std::vector<size_t> pos;
	std::vector<float> v;
	std::vector<std::vector<float>> vn;
	std::vector<float> toNext;
	midi(std::string s){		
		std::ifstream ifs("resources/"+s, std::ios::in | std::ios::binary);
		ifs>>std::noskipws;
		int i=0;		
		//static header
		str(ifs, 4, i);
		val(ifs, 4, i);

		tempoChanges.push_back(std::make_pair(0, 500000));

		format = val(ifs, 2, i);
		trackCount = val(ifs, 2, i);
		int td = val(ifs, 2, i);
		if((td & 0x8000) == 0){
			reltime = true;
			//ticks per quarter note
			timediv = (float)(td & 0x7FFF);
		} else {
			reltime = false;
			//ticks per second
			int hb = (td & 0x7F00) >> 4;
			
			int lb = (td & 0x00FF);

			if(hb == 29){
				timediv = (float)(29.97f*lb);
			} else {
				timediv = (float)(lb * hb);
			}
		}

		for(int p=0;p<trackCount;++p){
			tracks.push_back(track(ifs, reltime, timediv, tempoChanges));
		}

		for(size_t p=0;p<tracks.size();++p){
			pos.push_back(0);
			v.push_back(0);
			toNext.push_back(0);
			vn.push_back(std::vector<float>());

			for(int q=0;q<128;++q){
				vn.back().push_back(0);
			}
			std::cout<<p<<": "<<tracks[p].notes.size()<<std::endl;
		}

	}
	void run(float time){
		for(size_t i=0;i<tracks.size();++i){
			if(tracks[i].notes.size() == 0) continue;
			while(pos[i] > 0 && tracks[i].notes[pos[i-1]].second.first > time){
				pos[i]--;
			}
			bool p = false;
			while(pos[i] < tracks[i].notes.size() && tracks[i].notes[pos[i]].second.first < time){
				pos[i]++;
				if(p == false){
					v[i]+=0.4f;
					vn[i][tracks[i].notes[pos[i]-1].first] += 1.0f;
				}
				p = true;
				
			}
			
			if(pos[i] < tracks[i].notes.size()){
				toNext[i] = tracks[i].notes[pos[i]].second.first-time;
			} else {
				toNext[i] = 500;
			}
			v[i]*=0.95f;
			for(int j=0;j<128;++j){
				vn[i][j] *= 0.75f;
			}
		}
	}
};