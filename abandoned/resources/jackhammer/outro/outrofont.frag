
#version 150

out vec4 out_Color;
in vec2 uv;
uniform sampler2D text;
uniform float visibility;

void main(void)
{	
	vec4 g = texture(text, vec2(uv.x,1.f-uv.y));
	g *= visibility;
	out_Color = g;
}
