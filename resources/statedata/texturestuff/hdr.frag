#version 330

uniform sampler2DMS tex;
uniform sampler2DMS greetz;
in vec2 uv;
uniform float time;
out vec4 texOut;
uniform vec2 resolution;

void main(void)
{
	vec2 u = uv*2.0-1.0;

	vec3 gz = texelFetch(greetz, ivec2((vec2(0.0,1.0)+uv*vec2(1.0,-1.0))*vec2(480.0,320.0)), 0).rgb;

	float off = 0.0;//clamp(pow(pow(abs(u.x*0.9),4.0)+pow(abs(u.y*0.9),4.0),1.0/1.0),0.0,1.0);

	vec3 g = vec3(0.0); 
	for(int i=0;i<8;i++){
		g += texelFetch(tex, ivec2(gl_FragCoord.xy-(gl_FragCoord.xy-resolution/2.0)*off*0.2), i).rgb;
	}
	g/=8.0;
	//g*=1.1-length(u);
		//g=g/8.0;
	g.rgb += gz*0.05;

	texOut.xyz = g;

	texOut.a = 1.0;
}