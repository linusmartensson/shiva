#version 330

uniform float time;
in vec2 uv;
in vec4 p;
in vec4 op;
out vec4 c;
uniform float mark;
uniform float offset;
uniform float off;
uniform float on;
uniform sampler2D wform;

void main(void)
{
	c = vec4(1.0);
	c.r = uv.x*0.1;
	c.g = uv.x*4.0-2.0;
	//c.r /= op.z*0.05+0.5;
	c.r *= c.z/c.w;
	c.rg *= 5.0-clamp(op.z*100.0*(1.0+max(0.0,(time-64.0)*2.0))*clamp(time-on-2.0,-1.0,1.0)+40000-3000.0*time-texture2D(wform, vec2(0.01)).r*160000.0,0.0,5.0)+0.2;
	c.rgba *= clamp(time-on, 0.0, 1.0);
	c.rgba *= clamp(off-time, 0.0, 1.0);
	c.rgba *= 0.3+smoothstep(32.57,32.6,time)*0.2;
}
