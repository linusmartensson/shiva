
#version 330
in  vec3 pos;
in vec4 color;
out vec4 ocolor;
out vec4 opos;
out vec3 ipos;
void main(void)
{
	ipos = vec3(pos.xy, pos.z);
	opos = vec4(pos.xy, pos.z, 1.0);
	ocolor = color;
}