
#version 330
layout(location=0) out vec4 w;
layout(location=1) out vec4 a;
layout(location=2) out vec4 c;
in vec2 uv;
uniform mat4 camera;
uniform float time;
uniform mat3 rot;
uniform vec3 trans;
uniform float skip;
uniform sampler2D house;

void main(void)
{

	vec3 dir = vec3((vec3(uv.xy*2.0-1.0,1.0)*vec3(16.0/9.0,1.0,1.0)));
	
	dir = rot*dir;

	vec3 ray = vec3(0.0,0.0,0.0);
	ray += trans;
	w = vec4(ray+dir*100.0,1.0);
	
	c = texture2D(house, vec2(0.0,1.0)+uv*vec2(1.0,-1.0))*1.0;
	a = vec4(0.0,0.0,0.0,2.0+abs(dot(sin(uv+time),sin(uv*32142.0)))*2.0);	
	a.w *= 0.5;
	a.x = uv.x*cos(time)-uv.y*sin(time);
	a.y = uv.x*sin(time)+uv.y*cos(time);
	a.xy*=32.0;
	c.rgba *= 0.2;
}
