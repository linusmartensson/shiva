
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
	out_Color = vec4(color.rgb*0.5+0.5*vec3(dot(color.rgb,vec3(1.0))),1.0)*
		f*1.0+
		f*10.0*
			vec4(color.rgb/(0.1+abs(vec3(dot(color.rgb/distance,vec3(1.0))))),1.0)*texture2D(t, gl_PointCoord*vec2(0.2,1.0)+mod(vec2(float(vi)/6.0,0.0),vec2(1.0)));
	out_Color = 0.1*abs(out_Color)*clamp(time-2.0,0.0,5.0)*0.1;
}
