#pragma once 

#include <windows.h>
#include <Shlobj.h>
#include "NuiApi.h"
#include<boost/thread.hpp>
#include"log.h"

class nuicamera{
	bool ok;
	INuiSensor *sensor;
	boost::mutex mutex;
public:
	nuicamera(bool withColor = true, bool withDepth = false) : ok(false){
		sensor = 0;

		Log::info()<<"Attempting to start NUI camera."<<std::endl;

		int count;
		if(NuiGetSensorCount(&count) < 0){
			Log::errlog()<<"No sensor connected!"<<std::endl;
			return;
		}
		for(int i=0;i<count;++i){
			
			if(NuiCreateSensorByIndex(i, &sensor) < 0){
				continue;
			}

			if(sensor->NuiStatus()==S_OK){
				break;
			}

			sensor->Release();
			sensor = 0;
		}
		
		if(sensor == 0){
			Log::errlog()<<"No sensor available!"<<std::endl;
			return;
		}

		

		if(sensor->NuiInitialize((withColor?NUI_INITIALIZE_FLAG_USES_COLOR:0)|(withDepth?NUI_INITIALIZE_FLAG_USES_DEPTH:0)) < 0){
			Log::errlog()<<"Sensor initialization error!"<<std::endl;
			return;
		}
		
		//	sensor->NuiCameraElevationSetAngle(5);
		Log::info()<<"Sensor activated."<<std::endl;

		ok=true;
	}
	~nuicamera(){
		if(ok){
			sensor->NuiShutdown();
			sensor->Release();
		}
	}

	INuiSensor& camera() const {
		return *sensor;
	}

	bool available() const {
		return ok;
	}

	boost::mutex &lockable(){
		return mutex;
	}
};

class nuidata{
	HANDLE changeEvent[3];
	bool dying, changed[2];
	boost::thread t;	
	float *depth;
	byte *color;
	HANDLE depthhandle, colorhandle;
	nuicamera *nc;
	LONG *tcoords;
	float *colorCoordinates;
	
public:
	nuidata(nuicamera *nc) : dying(false), nc(nc) {
		//boost::unique_lock<boost::mutex> lock(nc->lockable());
		

		
		changed[0] = changed[1] = false;
		depthhandle = 0;
		colorhandle = 0;

		changeEvent[0] = CreateEvent(NULL, TRUE, FALSE, NULL); 
		changeEvent[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
		changeEvent[2] = CreateEvent(NULL, TRUE, FALSE, NULL);
		
		depth = static_cast<float*>(malloc(640*480*sizeof(float)));
		color = static_cast<byte*>(malloc(640*480*4));

		Log::info()<<"Opening stream"<<std::endl;

		if(nc->available() == false) return;

		if(nc->camera().NuiImageStreamOpen(
			NUI_IMAGE_TYPE_DEPTH,
			NUI_IMAGE_RESOLUTION_640x480,
			0,
			2,
			changeEvent[0],
			&depthhandle) < 0) return;

		//nc->camera().NuiImageStreamSetImageFrameFlags(depthhandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);

		if(nc->camera().NuiImageStreamOpen(
			NUI_IMAGE_TYPE_COLOR,
			NUI_IMAGE_RESOLUTION_640x480,
			0,
			2,
			changeEvent[1],
			&colorhandle) < 0) return;
		
		tcoords = new long[640*480*2];
		colorCoordinates = new float[640*480*2];

		Log::info()<<"Starting thread"<<std::endl;

		t = boost::thread([&]{
			while(!dying){
				int result = WaitForMultipleObjects(3, changeEvent, FALSE, INFINITE);
				
				if(dying) break;
				
				if(result == WAIT_OBJECT_0){
					depthinput();
				}
				if(result == WAIT_OBJECT_0+1){
					colorinput();
				}				
			}
		});

	}
	~nuidata(){
		dying = true;
		SetEvent(changeEvent[2]);
		t.join();
		free(depth);
		free(color);
		free(tcoords);
		free(colorCoordinates);
	}
	void colorinput(){
		//boost::unique_lock<boost::mutex> lock(nc->lockable());

		NUI_IMAGE_FRAME frame;
		if(nc->camera().NuiImageStreamGetNextFrame(colorhandle,0, &frame) < 0){
			return;
		}

		INuiFrameTexture *texture = frame.pFrameTexture;

		NUI_LOCKED_RECT locked;
		texture->LockRect(0, &locked, NULL, 0);

		if(locked.Pitch == 0){
			texture->UnlockRect(0);
			texture->Release();
			nc->camera().NuiImageStreamReleaseFrame(colorhandle, &frame);
			return;
		}

		if(!changed[1]){
			memcpy(color, locked.pBits, 640*480*4);
		}

		texture->UnlockRect(0);
		nc->camera().NuiImageStreamReleaseFrame(colorhandle, &frame);
		changed[1] = true;
	}
	void depthinput(){
		//boost::unique_lock<boost::mutex> lock(nc->lockable());

		NUI_IMAGE_FRAME frame;
		if(nc->camera().NuiImageStreamGetNextFrame(depthhandle,0, &frame) < 0){
			return;
		}

		INuiFrameTexture *texture = 0;
		BOOL nearMode;
		if(nc->camera().NuiImageFrameGetDepthImagePixelFrameTexture(
			depthhandle, &frame, &nearMode, &texture) < 0){
				nc->camera().NuiImageStreamReleaseFrame(depthhandle, &frame);
				return;
		}

		NUI_LOCKED_RECT locked;
		texture->LockRect(0, &locked, NULL, 0);

		if(locked.Pitch == 0){
			texture->UnlockRect(0);
			texture->Release();
			nc->camera().NuiImageStreamReleaseFrame(depthhandle, &frame);
			return;
		}

		const NUI_DEPTH_IMAGE_PIXEL *pos = reinterpret_cast<const NUI_DEPTH_IMAGE_PIXEL *>(locked.pBits);
		const NUI_DEPTH_IMAGE_PIXEL *end = pos + 640*480;

		float *i = depth;

		if(!changed[0]){
			while(pos<end) *(i++) = float(((pos++)->depth&0xFFFF));

			nc->camera().NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(
					NUI_IMAGE_RESOLUTION_640x480,NUI_IMAGE_RESOLUTION_640x480,
					640*480, (USHORT*)locked.pBits, 640*480*2, tcoords);
			for(int i=0;i<640*480*2;++i){
				colorCoordinates[i] = (float)tcoords[i];
			}
		}

		texture->UnlockRect(0);
		texture->Release();
		nc->camera().NuiImageStreamReleaseFrame(depthhandle, &frame);
		changed[0] = true;
	}
	void cleardepth(){changed[0] = false;}
	void clearcolor(){changed[1] = false;}
	bool peekdepth(){return changed[0];}
	bool peekcolor(){return changed[1];}
	float *depthdata(){
		if(changed[0] == false) return 0;
		return depth;
	}
	byte *colordata(){
		if(changed[1] == false) return 0;
		return color;
	}
	long *icolormap(){
		if(changed[0] == false) return 0;
		return tcoords;
	}
	float *colormap(){
		if(changed[0] == false) return 0;
		return colorCoordinates;
	}
};