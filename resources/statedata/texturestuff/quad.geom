#version 150
layout(points) in;
layout(triangle_strip, max_vertices = 10) out;

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
	//float d=sqrt(abs((m*vec4(ipos[0],1.0)).z))*0.1+1.0;
	float d=0.5;

	gl_Position = m*(vec4(-1.0,-1.0,0.0,0.0)*d+opos[0]);
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = m*(vec4(-1.0,1.0,0.0,0.0)*d+opos[0]);
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = m*(vec4(1.0,-1.0,0.0,0.0)*d+opos[0]);
    uv = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = m*(vec4(1.0,1.0,0.0,0.0)*d+opos[0]);
    uv = vec2(1.0,1.0);
	EmitVertex();
	
	EndPrimitive();
/*
	gl_Position = m*(vec4(-1.0,-1.0,0.0,0.0).zxyw*d+opos[0]);
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = m*(vec4(-1.0,1.0,0.0,0.0).zxyw*d+opos[0]);
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = m*(vec4(1.0,-1.0,0.0,0.0).zxyw*d+opos[0]);
    uv = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = m*(vec4(1.0,1.0,0.0,0.0).zxyw*d+opos[0]);
    uv = vec2(1.0,1.0);
	EmitVertex();
	
	EndPrimitive();
	*/
}
