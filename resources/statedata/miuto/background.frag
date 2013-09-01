
#version 330


in vec2 uv;
out vec4 c;
uniform float time;
uniform sampler2D wform;

uniform float on;
uniform float off;

void main(void)
{
	c = vec4(1.0);

	vec2 mid = uv*2.0-1.0;
	vec2 rd;
	
	mid.xy += 1.7;

	mid.xy *= 3.14159*0.5;





	rd = vec2(atan(mid.x,mid.y), sqrt(dot(mid,mid)));

	
	float t = time*time*0.5;
	float trd = sin(rd.y+t*clamp(rd.y/(t*t*0.1+0.1)-0.0,0.0,1.0));
	float f = texture2D(wform, vec2(trd*0.1,0.5)).r;
	rd.x = sin(rd.x*2.5+t*0.2+f*3.0);
	rd.y = trd;
	rd.y /= (10.0+rd.y);
	mid.x = sin(rd.x)*rd.y;
	mid.y = cos(rd.x)*rd.y;
	
	mid.xy = sin(mid.xy*5.0);

	vec2 ord = rd;
	rd = vec2(atan(mid.x,mid.y), sqrt(dot(mid,mid)));
	

	
	c.r = sin(rd.x)*rd.y*pow(f*10.0,2.0)*0.2;
	c.g = cos(rd.x)*rd.y*pow(f*10.0,2.0)*0.2;
	c.rgba *= clamp(time-on, 0.0, 1.0);
	c.rgba *= clamp(off-time, 0.0, 1.0);
	c *= 0.4;
}
