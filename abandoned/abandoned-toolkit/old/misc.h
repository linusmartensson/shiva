#pragma once

#include"texture.h"
#include"instance.h"
#include"framebuffer.h"
#include"shaders.h"

void blur(Texture in, Texture temp, Texture out){
	
		static Instance<FSShader> fss2 = FSShader("guassianvert.frag");
		static Instance<FSShader> fss3 = FSShader("guassianhoriz.frag");
		static FrameBuffer fb;
		
		fb.colorbuffer(0, temp);
		fss2->input("tex", in);
		fb.render((FSShader *)fss2);

		fb.colorbuffer(0, out);
		fss3->input("tex", temp);
		fb.render((FSShader *)fss3);
}