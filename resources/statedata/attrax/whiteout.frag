#version 330

uniform float on;
uniform float off;
uniform float fadetime;
uniform float time;

in vec2 uv;
out vec4 c;

void main(){
	
	c = vec4(smoothstep(on, on+fadetime, time)*(1.0-smoothstep(off-fadetime,off,time)))*10.0;

}
