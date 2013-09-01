
#version 150

in vec4 position;
in vec4 position2;
in vec4 color;
out vec4 gposition;
out vec4 gposition2;
out vec4 gcolor;
void main(void) {
	gposition = position;
	gposition2 = position2;
	gcolor = color;
}