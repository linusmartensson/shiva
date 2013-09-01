#version 330

uniform float time;
in vec2 uv;
in vec4 p;
in vec4 op;
out vec4 c;
uniform float mark;
uniform float offset;
uniform float slomoon;
uniform float on;
uniform float off;
uniform float slomooff;

void main(void)
{
	c = vec4(uv*0.2+p.xy*1.0,1.0,1.0);
	c.g += sin(p.z*0.01);
	float tt = 0.0;
	
	c = (c*0.05) ;

	c.rgba *= clamp(time-on, 0.0, 1.0);
	c.rgba *= clamp(off-time, 0.0, 1.0);

	c*=2.0;
}
