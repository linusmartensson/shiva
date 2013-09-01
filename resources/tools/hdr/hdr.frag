
#version 330

layout(location = 0) out vec3 texOut;
layout(location = 1) out vec3 accOut;
in vec2 uv;
uniform sampler2DMS tex;
uniform sampler2D blur;
uniform sampler2DMS acc;
uniform sampler2DMS depth;

uniform float exposure;
uniform float maxbright;
uniform float accumulation;
uniform float mixnew;
uniform float mixold;
uniform float mixbloom;
uniform float accnew;
uniform float accold;
uniform float accbloom;


void main(void)
{

	float focuslength = 10.0;
	float focusdist = 0.04;

	vec3 g=vec3(0.0);
	vec3 a=vec3(0.0);
	vec3 b=vec3(0.0);
	float d = 10.0;
	for(int i=0;i<8;i++){
		g += texelFetch(tex, ivec2(gl_FragCoord.xy), i).rgb;
		d = min(d, texelFetch(depth, ivec2(gl_FragCoord.xy), i).r);
	}
	g *= 0.125;

	float zn = 1.0;
	float zf = 50000.0;

	d = 2.0*zn*zf / (zf+zn-(zf-zn)*(2.0*d-1.0));
	d /= zf;
	float doa = clamp(-d*focuslength+focusdist*focuslength, 0.0, 1.0) + clamp(d*focuslength-focusdist*focuslength , 0.0,1.0);
	d = doa;
	
	
	
	b += texture2D(blur, uv).rgb;
	a += texelFetch(acc, ivec2(gl_FragCoord.xy), 0).rgb;
	//vec3 f = g*mixnew*(1.0-d)+b*mixbloom*d+a*mixold*(d);

	vec3 f = g*mixnew+b*mixbloom+a*mixold;

	texOut = f*exposure * (exposure / (maxbright + 1.0)) / (exposure + 1.0);
	accOut = g*accnew+b*accbloom+a*accold;
}
