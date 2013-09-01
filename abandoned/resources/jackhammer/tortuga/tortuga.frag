
#version 150

out vec3 c;
in vec2 uv;
in vec4 glp;
uniform float time;
uniform float fairy;

void main(void)
{
	vec4 g = glp;
	g *= 0.001;
	c=min(vec3(1000.0),vec3(g.z+g.z, g.z*fairy, g.z*g.z))*0.001 - abs(g.z*g.z*g.z)*0.00001;
	c = max(vec3(0.f), c);
}
