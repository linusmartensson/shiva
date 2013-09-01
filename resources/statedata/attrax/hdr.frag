#version 330

uniform sampler2D color;

uniform float time;

in vec2 uv;
out vec4 c;

void main(){

	vec4 co = max(texture2D(color, uv.xy),vec4(0.0));

	
	
	vec4 co2 = max(texture2D(color, uv.xy),vec4(0.0));
	
	float r = co.r;
	float g = co.g;
	r = r/(1.0+r);
	g = g/(1.0+g);

	c = vec4(
		-r+r*r*2.0+g*0.2, 
		r+g*0.1, 
		r-r*r+r*r*r*0.5+g*0.2, 1.0);

	
	vec4 q = c;
	vec4 qq = c;
	q.r *= 1.5;
	q.bg -= q.r*0.5;
	q.rgb*=2.5;

	c = mix(c,q,clamp(smoothstep(0.0,30.0,time),0.0,1.0));
	c = mix(c,(qq+q)*0.5,clamp(smoothstep(80.0,90.0,time),0.0,1.0));
}