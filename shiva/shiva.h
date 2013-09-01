#pragma once
#include"stdafx.h"
#include"utils.h"
#include"state.h"
#include"resourcetracker.h"
#include<algorithm>
#include<boost/thread.hpp>

#ifndef WGL_ARB_create_context
#define WGL_ARB_create_context 1
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
#define WGL_CONTEXT_MAJOR_VERSION_ARB		0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB		0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB		0x2093
#define WGL_CONTEXT_FLAGS_ARB			0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB		0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB		0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB	0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB	0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#endif

typedef const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hdc);

namespace shiva {
	static int isExtensionSupported( HDC hdc, const char *extension ) {
		PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress("wglGetExtensionsStringARB");
		if ( wglGetExtensionsStringARB == NULL )
		{
		  return FALSE;
		}
		const char *pWglExtString = wglGetExtensionsStringARB( hdc );
		return ( pWglExtString != NULL ) && ( strstr(pWglExtString, extension) != NULL );
	}
	enum FULLSCREEN_MODE{
		FULLSCREEN_DISABLED,
		FULLSCREEN_ENABLED,
		FULLSCREEN_ENABLED_WITH_DESKTOP_RESOLUTION
	};
	
	class threadpool{
		std::vector<boost::thread> threads;
		int cpus;
		std::vector<std::function<void()>> tasks;
		std::vector<bool> working;
		boost::mutex mutex;
		bool dying;
		int sleeping, dead;

		std::function<void()> fetch(int id){
			boost::unique_lock<boost::mutex> lock(mutex);
			if(tasks.size()==0){
				if(working[id] == true){
					sleeping++;
				}
				working[id] = false;
				return std::function<void()>([]{boost::this_thread::yield();});
			}
			auto f = tasks.back();
			tasks.pop_back();
			return f;
		}
		void run(int id){
			while(!dying){
				fetch(id)();
			}
			{
				boost::unique_lock<boost::mutex> lock(mutex);
				dead++;
			}
		}

	public:
		threadpool() : cpus(0), dying(false), sleeping(0), dead(0) {

			if((cpus = boost::thread::hardware_concurrency()) == 0){
				Log::info()<<"Warning: could not fetch CPU count. Defaulting to two threads in thread pool.";
				cpus = 2;
			}
			for(int i=0;i<cpus;++i){
				working.push_back(true);
				threads.push_back(boost::thread([i, this]{this->run(i);}));
			}
		}
		void add(std::vector<std::function<void()>> fs){
			boost::unique_lock<boost::mutex> lock(mutex);
			std::copy(fs.begin(),fs.end(),std::back_inserter(tasks));
			for(size_t i=0;i<working.size();++i){working[i] = true;}
			sleeping = 0;
		}
		void waitDone(){
			while(sleeping != cpus) boost::this_thread::yield();
		}
		void die(){
			dying = true;
			while(dead != cpus) boost::this_thread::yield();
		}
	};

	class core{
		static HGLRC primaryContext;
		static HDC deviceContext;

		static threadpool *pool;
		static state *root;

		static uint _width, _height;
		static int wheelpos;
		static int lx, ly;
		static int currentChar;
		static std::set<int> downkeys, repeatkeys, mousekeys;
	
		static boost::shared_mutex mutex;
		static bool hasEvent;

		static resourcetracker rt;

		static bool dying;

	public:
		static void stop(){dying = true;}

		
		static bool down(int key){return downkeys.count(key)>0;}
		static bool repeat(int key){return repeatkeys.count(key)>0;}
		static bool mouse(int key){return mousekeys.count(key)>0;}
		static int ch(){return currentChar;}
		static uint width(){return _width;}
		static uint height(){return _height;}
		static HGLRC primaryRenderingContext(){return primaryContext;}
		
		static void on(std::string s, const boost::any &d){
			{
				boost::upgrade_lock<boost::shared_mutex> lock(mutex);
				if(hasEvent == false){
					boost::upgrade_to_unique_lock<boost::shared_mutex> ulock(lock);
					hasEvent = true;
				}
			}
			if(root) root->touch(s, d);
		}

		static int GLFWCALL fixclose(){core::stop();return GL_FALSE;}
		static void resize(int w, int h){_width = w;_height = h; on("resize", std::make_pair(w, h));}
		static void winput(int pos){on("winput", pos>wheelpos); wheelpos = pos;}
		static void binput(int btn, int state){if(state==GLFW_PRESS) mousekeys.insert(btn); else mousekeys.erase(btn); on("binput", std::make_pair(btn,state==GLFW_PRESS));}
		static void minput(int x, int y){lx = x;ly = y; on("minput", std::make_pair(x,y));}
		static void kinput(int key, int state){if(state==GLFW_PRESS){ repeatkeys.insert(key); downkeys.insert(key); } else downkeys.erase(key); on("kinput", std::make_pair(key, state==GLFW_PRESS));}
		static void cinput(int key, int state){if(state==GLFW_PRESS) currentChar=key; on("cinput", std::make_pair(key, state==GLFW_PRESS));}
		
		static void exit(int i){
			std::cout<<"Press enter to exit..."<<std::endl;
			getchar(); ::exit(i);
		}

		static void shiva::core::run(std::string behaviour, std::string windowname, FULLSCREEN_MODE fullScreen, int w, int h,int fsaaLevel,bool vsync)
		{
			//Static data initialization.
			GLFWvidmode mode;

			glfwInit();
			glfwGetDesktopMode(&mode);
			glfwOpenWindowHint(GLFW_FSAA_SAMPLES, fsaaLevel);
			glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
			glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
			glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			int err = glfwOpenWindow( 
				_width = (fullScreen==FULLSCREEN_ENABLED_WITH_DESKTOP_RESOLUTION?mode.Width:w), 
				_height = (fullScreen==FULLSCREEN_ENABLED_WITH_DESKTOP_RESOLUTION?mode.Height:h),
				mode.RedBits, mode.GreenBits, mode.BlueBits, 8, 32, 0, fullScreen?GLFW_FULLSCREEN:GLFW_WINDOW );
			if(err == GL_FALSE){
				std::cout<<"glfwOpenWindow failed, aborting."<<std::endl;
				exit(1);
			}

			primaryContext = wglGetCurrentContext();
			deviceContext = wglGetCurrentDC();
	
			if (gl3wInit() || !gl3wIsSupported(3,3)) {
				std::cout << "gl3wInit failed, aborting." << std::endl;
				exit(1);
			}
	
			glfwDisable(GLFW_MOUSE_CURSOR);
			glfwEnable(GLFW_KEY_REPEAT);
			glfwSetWindowTitle(windowname.c_str());
			glfwSwapInterval(vsync?1:0);
	
			std::cout << "gl3w initialized successfully." << std::endl;
			std::cout << "OpenGL " << glGetString(GL_VERSION) << " running on " << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << std::endl;
			std::cout << "GLSL version: "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;
	
			glfwSetWindowCloseCallback(core::fixclose);
			glfwSetWindowSizeCallback(core::resize);
			glfwSetKeyCallback(core::kinput);
			glfwSetCharCallback(core::cinput);
			glfwSetMousePosCallback(core::minput);
			glfwSetMouseButtonCallback(core::binput);
			glfwSetMouseWheelCallback(core::winput);
	
			Log::GLErrors("");
			pool = new threadpool();
			root = state::create("root", behaviour, 0);

			on("init", 0);

			long startTime = (long)(glfwGetTime()*1000);
			long currentTime = startTime;

			while(dying==0){
				if(rt.peek()) on("resourcechange", 1);
				on("frame", 0);
				rt.clear();
				
				//Run until no new events exist.
				do {
					hasEvent = false;
					std::vector<state*> states = state::states();
					std::vector<std::function<void()>> tasks;
					std::for_each(states.begin(),states.end(),[&](state *s){
						tasks.push_back(std::function<void()>([s](){s->run();}));
					});
					pool->add(tasks);
					pool->waitDone();
				} while(hasEvent);

				root->render();
				Log::GLErrors("Unhandled GL error after rendering.");
				glfwSwapBuffers();
			}
			on("exit", 0);
			{//Loop once more for exit event handling.
				std::vector<state*> states = state::states();
				std::vector<std::function<void()>> tasks;
				std::for_each(states.begin(),states.end(),[&](state *s){
					tasks.push_back(std::function<void()>([s](){s->run();}));
				});
				pool->add(tasks);
				pool->waitDone();
				pool->die();
			}
			root = 0;
			state::destroy("root");
	
			delete pool;
			pool = 0;

			glfwEnable(GLFW_MOUSE_CURSOR);
			glfwTerminate();

			std::cout<<"Shiva de-initialized."<<std::endl;
		}
	};
}

