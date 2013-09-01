#version 330
layout(lines) in;
layout(line_strip, max_vertices = 10) out;

in vec4 opos[];
in vec3 ipos[];
in vec4 ocolor[];
out vec4 color;
out vec2 uv;
out vec3 pos;
uniform mat4 m;

void main() {
	color = ocolor[0];
	pos = ipos[0];
	uv = vec2(0.0);
	
	gl_Position = (opos[0])*m;
    EmitVertex();
	
	color = ocolor[1];
	pos = ipos[1];
	gl_Position = (opos[1])*m;
	EmitVertex();
	
	
	EndPrimitive();
}
