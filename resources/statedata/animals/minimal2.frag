
#version 150

out vec4 out_Color;
in vec4 color;
in float distance;
in float sz;
in float dd;

void main(void)
{
	float f = max(0.0,1.0-length(gl_PointCoord-0.5)*2.0);
	float b = max(20.0-distance,10.0);
	out_Color = vec4(color.rgb,1.0)*
		clamp(f*b,0.0,1.0)*(b*0.0025+0.25);
}
