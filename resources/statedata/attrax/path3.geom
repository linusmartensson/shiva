#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform float count;
in vec4 opos[];
in vec4 rpos[];
in float vid[];
out vec2 uv;
out vec4 p;
out vec4 op;
uniform float blink;
uniform float mark;

void main() {
	if((blink<0.5 && mark<0.5) || mod(vid[0]*0.5,4.0)<1.5){
		for(int i=0;i<3;++i){
			gl_Position = opos[i];	
			uv = vec2(vid[i]/count,0.0);
			p = rpos[i];
			op = opos[i];
			EmitVertex();
		}	
	}
	EndPrimitive();
}
