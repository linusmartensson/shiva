#version 150
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


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
	vec4 z = m*opos[0];
	if(pos.z<1&&pos.z>-1) return;
	
	gl_Position = m*(opos[0]);
	uv = vec2(0.0,0.0);
    EmitVertex();

	color = ocolor[1];
	pos = ipos[1];
	z = m*opos[1];
	if(pos.z<1&&pos.z>-1) return;
	
	gl_Position = m*(opos[1]);
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	color = ocolor[2];
	pos = ipos[2];
	z = m*opos[2];
	if(pos.z<1&&pos.z>-1) return;

	gl_Position = m*(opos[2]);
    uv = vec2(1.0,0.0);
	EmitVertex();

	EndPrimitive();
}
