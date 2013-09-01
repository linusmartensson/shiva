#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 24) out;


uniform mat4 camera;
out vec4 ppo;
in vec4 opos[];
in vec4 state[];
out vec4 glp;
out vec2 uv;
out vec4 s;

void main() {

	s = state[0];

	vec4 scale = vec4(vec3(10.0),0.0);
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,0.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,0.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,0.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	

	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,0.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();

	

	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,1.0,1.0));
	glp = gl_Position;
	uv = vec2(0.0,0.0);
	gl_Position *= camera;
    EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,0.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	
	s = state[0]; 
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,0.0);
	EmitVertex();
	
	s = state[0];
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
	gl_Position *= camera;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
}
