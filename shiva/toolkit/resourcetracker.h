#pragma once
#include"stdafx.h"
#include"state.h"
#include<functional>

class resourcetracker{
	HANDLE changeHandle[2];
	bool dying, changed;
	boost::thread t;

public:
	resourcetracker() : dying(false), changed(false) {
		TCHAR buf[4096];
		TCHAR **lpp  = {NULL};
		GetFullPathName(L"resources/statedata", 4096, buf, lpp);
		changeHandle[0] = FindFirstChangeNotification(buf, TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE); 
		changeHandle[1] = CreateEvent(NULL, TRUE, FALSE, NULL); 
		t = boost::thread([&]{
			while(!dying){
				WaitForMultipleObjects(2, changeHandle, FALSE, INFINITE);
				if(dying) break;
				change();
				FindNextChangeNotification(changeHandle[0]);
			}
		});
	}
	~resourcetracker(){
		dying = true;
		SetEvent(changeHandle[1]);
		t.join();
	}
	void change(){changed = true;}
	void clear(){changed = false;}
	bool peek(){return changed;}
};