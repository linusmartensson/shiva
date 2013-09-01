#version 330

uniform float time;
in vec2 uv;
in vec4 p;
in vec4 op;
out vec4 c;
uniform float mark;
uniform float offset;
uniform float off;

void main(void)
{
	c.a = 1.0;
	c.r = (1.0-mark)*100.0;
	c.r += mark*0.1;
	c.rg -= p.y*0.01;
	c.rg /= 1.0+(1.0+op.z*100.0)*0.001;

	c.rg *= clamp(1.0-smoothstep(off-5.0, off-1.0, time), 0.0,1.0);
	c.rgba *= clamp(op.z*0.01,0.0,1.0);
}
