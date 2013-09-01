
#version 330

layout(location=0) out vec4 w;
layout(location=1) out vec4 a;
layout(location=2) out vec4 c;
uniform float time;
in vec4 ppo;


void main(void)
{
	w = vec4(ppo.xyz,cos(ppo.x*431.351+ppo.y*10431.0+ppo.z*1351.0+time)*0.1);
	a = vec4(0.0*vec3(cos(ppo.x*431.351+ppo.y*10431.0+ppo.z*1351.0+time)*100.1),1.0+cos(ppo.x*131.351+ppo.y*20431.0+ppo.z*3351.0+time)*0.1);
	c = vec4(10.0,20+20.0*abs(sin(ppo.y*0.01))	,20.0,1.0);
}
