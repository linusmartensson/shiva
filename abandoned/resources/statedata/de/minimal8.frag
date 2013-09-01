
#version 330

layout(location=0) out vec4 w;
layout(location=1) out vec4 a;
layout(location=2) out vec4 c;
uniform float time;
in vec4 glp;
in vec2 uv;
in vec4 s;

void main(void)
{
	w = vec4(glp.xyz,cos(glp.x*431.351+glp.y*10431.0+glp.z*1351.0+time)*0.1);
	a = vec4(0.0,0.0,0.0,40.0+cos(glp.x*131.351+glp.y*20431.0+glp.z*3351.0+time)*1.1);
	vec2 u = uv*2.0-1.0;
	c.rgb += s.x*60.0;
	a.w += (1.0-s.x)*20000.0;
	a.y = clamp(c.r,0.0,30.0);
}
