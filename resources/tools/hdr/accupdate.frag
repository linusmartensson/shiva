
#version 150

out vec3 out_Color;
in vec2 uv;
uniform sampler2D tex;
uniform float exposure;
uniform float maxbright;

void main(void)
{
	vec3 g=vec3(0.0);
	g = texture2D(tex, uv).rgb;
	out_Color = g;

}
