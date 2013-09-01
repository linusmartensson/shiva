#version 330
in vec2 uv;
out vec4 texOut;
uniform sampler2D img;

void main(void)
{
	vec4 j = texture2D(img, vec2(0.0,1.0)+uv.xy*vec2(1.0,-1.0)).gbar; 
	texOut = j;
	texOut.a = 1.0;
}