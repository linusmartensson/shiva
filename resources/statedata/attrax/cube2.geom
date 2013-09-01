#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 25) out;

uniform float count;
in vec4 opos[];
in vec4 rpos[];
in float vid[];
out vec2 uv;
out vec4 p;
out vec4 op;
uniform float blink;
uniform float mark;
uniform mat4 camera;
uniform float time;
uniform float offset;
uniform float slomoon;
uniform float slomooff;
uniform sampler2D spectrum;
void main() {

	float tt = 0.0;

	

	tt = clamp(smoothstep(slomoon-1.0, slomoon, time),0.0,1.0) * (slomoon + (slomoon - time)*0.1) 
   *(1.0-clamp(smoothstep(slomooff-5.0, slomooff, time),0.0,1.0)) +
	    +(1.0-clamp(smoothstep(slomoon-1.0, slomoon, time),0.0,1.0))*time
		+clamp(smoothstep(slomooff-5.0, slomooff, time),0.0,1.0)*(time - (slomooff-slomoon)*0.9);
	vec4 scale=vec4(vec3(max(0.0,sin(rpos[0].z*0.002-tt+offset)))*8.0,1.0);

	scale += texture2D(spectrum,vec2(mod(vid[0]/count,0.2)+0.3,0.5)).rrrr*5000.0;

	op = opos[0];
	p = rpos[0];
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,1.0,1.0))*camera;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,1.0,1.0))*camera;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,1.0,1.0))*camera;
    uv = vec2(1.0,0.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,1.0,1.0))*camera;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,-1.0,1.0))*camera;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,-1.0,1.0))*camera;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,-1.0,1.0))*camera;
    uv = vec2(1.0,0.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,-1.0,1.0))*camera;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,1.0,1.0))*camera;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,1.0,1.0))*camera;
	uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,-1.0,1.0))*camera;
    uv = vec2(1.0,0.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,-1.0,1.0))*camera;
	uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,-1.0,1.0))*camera;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,-1.0,1.0))*camera;
	uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,1.0,1.0))*camera;
	uv = vec2(1.0,0.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,1.0,1.0))*camera;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();

	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,1.0,1.0))*camera;
	uv = vec2(0.0,0.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(-1.0,1.0,-1.0,1.0))*camera;
	uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,1.0,1.0))*camera;
	uv = vec2(1.0,0.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,1.0,-1.0,1.0))*camera;
	uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,1.0,1.0))*camera;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(-1.0,-1.0,-1.0,1.0))*camera;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,1.0,1.0))*camera;
    uv = vec2(1.0,0.0);
	EmitVertex();
	
	gl_Position = (opos[0]+scale *vec4(1.0,-1.0,-1.0,1.0))*camera;
    uv = vec2(1.0,1.0);
	EmitVertex();
	EndPrimitive();
}
