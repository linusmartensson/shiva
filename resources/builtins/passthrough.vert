
#version 330
in  vec3 pos;
uniform mat4 mat;
out vec4 geopos;
out vec4 opos;
out int gid;
void main(void)
{
	opos = vec4(pos, 1.0f);
	geopos = opos;
	gid = gl_VertexID;
}