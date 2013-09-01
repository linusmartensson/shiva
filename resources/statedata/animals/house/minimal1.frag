
#version 150

out vec4 out_Color;
in vec4 color;
in float pos;
uniform float time;
void main(void)
{
	float p = clamp(pow(1.0-abs(pos-0.5)*2.0,0.5),0.0,1.0)*10.0;
	out_Color = vec4(color.xyz*color.w*p+length(color.xyz),1.0);
}
