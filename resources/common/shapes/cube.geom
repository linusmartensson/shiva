#version 150
layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

out vec2 uv;
uniform mat4 mat;
in vec4 opos[];
out vec4 glp;

void main() {
	
	gl_Position = mat*(opos[0]+vec4(-1.0,-1.0,1.0,1.0));
	glp = gl_Position;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(-1.0,1.0,1.0,1.0));
	glp = gl_Position;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(1.0,-1.0,1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = mat*(opos[0]+vec4(1.0,1.0,1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = mat*(opos[0]+vec4(-1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(-1.0,1.0,-1.0,1.0));
	glp = gl_Position;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = mat*(opos[0]+vec4(1.0,1.0,-1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	
	
	gl_Position = mat*(opos[0]+vec4(1.0,-1.0,1.0,1.0));
	glp = gl_Position;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(1.0,1.0,1.0,1.0));
	glp = gl_Position;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = mat*(opos[0]+vec4(1.0,1.0,-1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	


	gl_Position = mat*(opos[0]+vec4(-1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(-1.0,1.0,-1.0,1.0));
	glp = gl_Position;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(-1.0,-1.0,1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = mat*(opos[0]+vec4(-1.0,1.0,1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();

	

	
	gl_Position = mat*(opos[0]+vec4(-1.0,1.0,1.0,1.0));
	glp = gl_Position;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(-1.0,1.0,-1.0,1.0));
	glp = gl_Position;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(1.0,1.0,1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = mat*(opos[0]+vec4(1.0,1.0,-1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = mat*(opos[0]+vec4(-1.0,-1.0,1.0,1.0));
	glp = gl_Position;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(-1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = mat*(opos[0]+vec4(1.0,-1.0,1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = mat*(opos[0]+vec4(1.0,-1.0,-1.0,1.0));
	glp = gl_Position;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
}
