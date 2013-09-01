#version 330

uniform float time;
in vec2 uv;
in vec4 p;
in vec4 op;
in float f;
out vec4 c;
uniform float on;
uniform float mark;
uniform float offset;
uniform float off;
in float vid;
void main(void)
{
	c.a = 1.0;

	c.r = 1.0;
	c.g = 10.0;

	c.rg *= clamp(-uv.x*10.0+time-on,0.0,1.0);

	c.rg *= max(sign(offset),0.0)*0.95+0.05;
	c *= 1.0-clamp(smoothstep(off-5.0,off,time), 0.0,1.0);
}
