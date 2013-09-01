#version 330
in vec4 color;
out vec4 texOut;

void main(void)
{
	texOut.w = 1.0;
	texOut.rgb=color.rgb/155.0;
}