
#version 150
in  vec3 pos;
uniform mat4 mat;
out vec4 geopos;
out vec4 opos;
out int id;
void main(void)
{
	geopos = mat*vec4(pos,1.0);
	opos = vec4(pos,1.0);
	
	id = gl_VertexID;
}