#version 330

in vec2 uv;
out vec4 c;
uniform float on;
uniform float off;
uniform float time;
uniform sampler2D text3;
float mr(float x, float y, float t, float fac){
	float r = atan(x, y);
	float d = sqrt(x*x+y*y);
	
	r+=t;
	d*=fac;

	x = sin(r)*d;

	return abs(x);

}

void main(){
	c.rgba = texture2D(text3, vec2(0.0,1.0)+uv*vec2(1.0,-1.0)).aaaa*0.5;
	c.rgba *= clamp(time-on, 0.0, 1.0);
	c.rgba *= clamp(off-time, 0.0, 1.0);

	c.rgba *= sin((time-on)*3.0)*0.5+0.5;

	if(c.r < abs(0.05)) discard;
}