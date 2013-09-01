
#version 330

in vec4 position;
in vec4 astate;
out vec4 gposition;
out vec4 opos;
out vec4 state;
uniform mat4 camera;
uniform float time;
void main(void) {
	opos = position;
	state = astate;
}