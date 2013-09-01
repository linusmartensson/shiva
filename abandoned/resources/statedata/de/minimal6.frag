
#version 330

layout(location=0) out vec4 w;
layout(location=1) out vec4 a;
layout(location=2) out vec4 c;
uniform float time;
in vec4 glp;
in vec2 uv;
in vec4 s;
uniform float inv;

void main(void)
{
	w = vec4(glp.xyz,cos(glp.x*431.351+glp.y*10431.0+glp.z*1351.0+time)*0.1);
	a = vec4(0.0,100.0,0.0,4.0+cos(glp.x*131.351+glp.y*20431.0+glp.z*3351.0+time)*1.1);
	vec2 u = uv*2.0-1.0;
	
	float v = dot(pow(abs(u),vec2(16.0)),vec2(1.0))*100.0;
	c = vec4(vec3(v),1.0);
	c.r = (s.x>0.0?100.f:c.r);
	if(inv > 0.5){

		c.rgba = vec4(s.x<-0.97?vec4(clamp(float(s.x>0.0)*100.f,-100.f,100.f),0.0,0.0,0.0):c.rgba);
	}else {
		c.rgba = vec4(s.x>-0.97?vec4(clamp(float(s.x>0.0)*100.f,-100.f,100.f),0.0,0.0,0.0):c.rgba);
	}
	c.rg *= 5.0;
	c.gb *= 0.5;

	c.rgb *= clamp(324-time,0.0,1.0);
}
