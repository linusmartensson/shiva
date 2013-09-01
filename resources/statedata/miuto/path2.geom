#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 camera;
uniform float count;
uniform float time;
in vec4 opos[];
in vec4 rpos[];
in float vid[];
out vec2 uv;
out vec4 p;
out vec4 op;

void main() {

	float mx = mod(sin(vid[0]+vid[1])+time*sin(vid[1]),sin(vid[0])+2.0);

	if(mx <= 1.0){

		vec4 pp = mix(opos[0],opos[1], mx);
	
		float sc = 0.5;

		uv = vec2(mix(vid[0], vid[1], mx)/count,0.0);
		p = mix(rpos[0], rpos[1], mx);
		vec4 fp = pp*camera + vec4(1.0,-1.0,0.0,0.0)*sc;
		op = fp;
		gl_Position = fp;
		EmitVertex();
	
		uv = vec2(mix(vid[0], vid[1], mx)/count,0.0);
		p = mix(rpos[0], rpos[1], mx);
		fp = pp*camera + vec4(1.0,1.0,0.0,0.0)*sc;
		op = fp;
		gl_Position = fp;
		EmitVertex();

		uv = vec2(mix(vid[0], vid[1], mx)/count,0.0);
		p = mix(rpos[0], rpos[1], mx);
		fp = pp*camera + vec4(-1.0,-1.0,0.0,0.0)*sc;
		op = fp;
		gl_Position = fp;
		EmitVertex();

		uv = vec2(mix(vid[0], vid[1], mx)/count,0.0);
		p = mix(rpos[0], rpos[1], mx);
		fp = pp*camera + vec4(-1.0,1.0,0.0,0.0)*sc;
		op = fp;
		gl_Position = fp;
		EmitVertex();

		EndPrimitive();
	}
}
