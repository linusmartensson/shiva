
#version 150

out vec4 out_Color;
in vec4 color;
in float distance;
in float sz;
in float dd;

flat in int vi;
uniform sampler2D t;
uniform float time;

void main(void)
{
	float f = max(0.0,1.0-length(gl_PointCoord-0.5)*2.0);
	float b = max(20.0-distance,10.0);
	out_Color = color.rgba*(1.0-step(f,0.0));
}
