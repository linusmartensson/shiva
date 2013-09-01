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

void main(void)
{
	c = vec4(uv*0.2+p.xy*1.0,1.0,1.0);
	c.g += sin(p.z*0.01);
	float tt = 0.0;
	
	c = clamp(smoothstep(slomoon-1.0, slomoon, time),0.0,1.0) * (c*0.05) 
   *(1.0-clamp(smoothstep(slomooff-5.0, slomooff, time),0.0,1.0)) +
	    +(1.0-clamp(smoothstep(slomoon-1.0, slomoon, time),0.0,1.0))*c*0.3
		+clamp(smoothstep(slomooff-5.0, slomooff, time),0.0,1.0)*(c*0.15);

	c *= clamp(1.0-smoothstep(off-5.0,off,time),0.0,1.0);
}
