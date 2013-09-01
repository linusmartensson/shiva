#version 330

uniform float time;
in vec2 uv;
in vec4 p;
in vec4 op;
out vec4 c;
uniform float mark;
uniform float offset;
uniform float slomoon;
uniform float off;
uniform float slomooff;
uniform float on;
void main(void)
{
	c = vec4(uv*0.2+p.xy*1.0,1.0,1.0);
	c.g += sin(p.z*0.01);
	float tt = 0.0;
	
	c = c*0.055;

	c *= clamp(1.0-smoothstep(off-5.0,off,time),0.0,1.0);
	c *= clamp(smoothstep(on, on+5.0, time),0.0,1.0);
}
