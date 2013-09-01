#version 330

layout(lines) in;
layout(line_strip, max_vertices = 2) out;

uniform float count;
in vec4 opos[];
in vec4 rpos[];
in float vid[];
out vec2 uv;
out vec4 p;
out vec4 op;

void main() {
	for(int i=0;i<2;++i){
		gl_Position = opos[i];	
		uv = vec2(vid[i]/count,0.0);
		p = rpos[i];
		op = opos[i];
		EmitVertex();
	}	
	EndPrimitive();
}
