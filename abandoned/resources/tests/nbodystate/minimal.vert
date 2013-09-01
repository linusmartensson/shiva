
#version 150
in  vec4 in_Position;
uniform mat4 rot;
out vec3 geo_Color;
out vec4 geo_Position;
out float z;
void main(void)
{
	geo_Position = vec4(in_Position.xy, 1.0, 1.0);
	z = in_Position.z*0.01f;
}