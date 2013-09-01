
#version 150

in vec4 position;
in vec4 position2;
in vec4 color;
in vec4 acc;
out vec4 gposition;
out vec4 gposition2;
out vec4 gcolor;
out int vid;
out vec4 gacc;
void main(void) {
	gposition = position;
	gposition2 = position2;
	gcolor = color;
	gacc = acc;
	vid = gl_VertexID;
}