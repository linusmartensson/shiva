
#version 150

out vec4 out_Color;
in vec4 color;

void main(void)
{
	out_Color = vec4(color.xyz*color.w*3.0*7.0,1.0);
}
