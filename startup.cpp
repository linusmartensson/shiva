#include"stdafx.h"
#include"shiva.h"
//#include"state/dq/th.h"
//#include"state/dq/tj.h"
#include"state/dq/tj.h"
//#include"state/dq/root.h"
//#include"state/miuto/hdr.h"
//#include"state/texturestuff/tk.h"
//#include"state/partevo/root.h"
#include<sstream>

boost::shared_mutex shiva::core::mutex;
bool shiva::core::hasEvent;

int main (int argc, char* argv[])
{
/*	int width = 1920;
	int height = 1080;*/
	int width = 1280;
	int height = 720;
	int fsaa = 8;
	if(argc >= 3){
		std::string w(argv[1]);
		std::string h(argv[2]);
		std::istringstream iw(w);
		std::istringstream ih(h);
		iw>>width;
		ih>>height;
		if(argc >= 4){
			std::string f(argv[3]);
			std::istringstream ii(f);
			ii>>fsaa;
		}
	}
	
	shiva::core::run("root", "3d1T0r", shiva::FULLSCREEN_DISABLED, width, height, fsaa, true);
	//shiva::core::run("root", "blur", shiva::FULLSCREEN_ENABLED, width, height, fsaa, true);
	return 0;
}