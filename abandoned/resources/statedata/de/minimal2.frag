
#version 150

out vec4 out_Color;
in vec4 color;
in float distance;
in float sz;
in float dd;

void main(void)
{
	float f = 1.0-length(gl_PointCoord-0.5)*2.0;
	
	out_Color = vec4(color.xyz*color.w*1.0*7.0,1.0)*clamp(f*(1.0-distance)*20.0,0.0,1.0);
	out_Color = max(out_Color*(distance+1.0),0.0)*max(20.0-distance,10.0);
	out_Color.a = 2.0/sqrt(dot(out_Color.rgb, vec3(1.0)));
	out_Color.rgba = max(out_Color.rgba,0);
}
