
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

	
	float t = time*0.5;
	float trd = sin(rd.y+clamp(rd.y/(t*t*0.1+0.1)-0.0,0.0,1.0));
	rd.x = sin(rd.x*.5+t*0.2);
	rd.y = rd.y*0.2+rd.x*0.2;
	mid.x = sin(rd.x)*rd.y;
	mid.y = cos(rd.x)*rd.y;
	
	mid.xy = sin(mid.xy*2.0);
	

	
	c.r = mid.x*0.02;
	c.g = mid.y*0.02;
	c.rgba *= clamp(time-on, 0.0, 1.0);
	c.rgba *= clamp(off-time, 0.0, 1.0);
}
