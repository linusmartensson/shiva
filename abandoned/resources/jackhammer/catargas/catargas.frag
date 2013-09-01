
#version 150

out vec3 c;
in vec4 pos;
in vec4 gpos;

uniform float distort;

void main(void)
{
	vec2 pc = (gl_PointCoord.xy-vec2(0.5,0.5))*2.0;
	float p = sqrt(pc.x*pc.x+pc.y*pc.y);
	c = vec3(3.0,
			1.0 ,0.0);
}
