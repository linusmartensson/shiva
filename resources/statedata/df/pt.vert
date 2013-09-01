
#version 330
in  vec3 pos;
in vec4 color;
out vec4 ocolor;
out vec4 opos;
out vec3 ipos;
void main(void)
{
	ipos = vec3(-vec2(320.0,240.0)+pos.xy, pos.z);
	opos = vec4(pos.xy	, pos.z, 1.0);
	ocolor = color-length(pos.xyz)*0.01;
}