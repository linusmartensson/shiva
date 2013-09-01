
#version 330

in vec4 position;
out vec4 gposition;
out vec4 pp;
uniform mat4 camera;
uniform float time;
void main(void) {
	gposition = position*camera;
	pp = position;
}