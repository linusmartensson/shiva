
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
	a = vec4(0.0,200.0,0.0,clamp(10.0-clamp(time-80.0,0.0,20.0)+cos(glp.x*131.351+glp.y*20431.0+glp.z*3351.0+time)*1.1,0.1,100.0));
	vec2 u = uv*2.0-1.0;
		
	float v = 1.0;//max(dot(pow(abs(u),vec2(16.0)),vec2(1.0))*100.0,0.0)*clamp(60-time,0.0,1.0);

	if(s.y <0.5)
		c.rgb = vec3(9.0,s.x*50.0,1.0)*2.0;
	else
		c.rgb = vec3(6.0+sin(time*3.0)*3+cos(time*4.0)*2+max(time-7.2,0.0)*10.0);
	c.a = 1.0;
	w.w=2.0;
}
