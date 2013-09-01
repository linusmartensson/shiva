#include<map>
#include"texture.h"
#include"framebuffer.h"
#include"shaders.h"
class resources{

	std::map<std::string, Texture> textures;
	FrameBuffer fb;

	template<typename T>
	void create(std::string name, int w, int h, std::function<T> fcn){
		Texture t(1, GL_RGBA32F, GL_TEXTURE_2D, w, h);
		fb.colorbuffer(0, t);
		fb.render(fcn);
		textures[name] = t;
	};
	template<typename T>
	void create(std::string name, int w, int h, std::string fsshader, std::function<T> fcn){
		FSShader fss(fsshader);
		create(name, w, h, [fss](){
			fcn(&fss);
			fss.display();
		});
	};
};