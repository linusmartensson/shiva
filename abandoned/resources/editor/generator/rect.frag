
#version 150

uniform float active;

out vec4 out_Color;
in vec2 uv;

void main(void)
{
	out_Color = vec4(0.4,0.4,0.4,1.0)+vec4(vec3(active*0.3),1.0);
}
