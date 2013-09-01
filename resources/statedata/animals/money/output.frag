
#version 330
out vec4 o;
in vec2 uv;
uniform sampler2D w;
uniform sampler2D a;
uniform sampler2D c;
uniform float time;
uniform float vis;
uniform float range;
uniform vec2 resolution;

void main(void)
{
	
	vec4 aa = texture2D(c,uv);
	
	o=aa*aa.a/(1.0+time);
	if(distance(texture2D(w,uv).xyz,vec3(0.0)) > range) o = vec4(0.0,0.0,0.0,1.0);
	
	o.rgb = o.rgb*1.0+(1.0-length(uv-0.5))*10.0 
	+ sin(uv.y*3.14159*2.0*resolution.y*0.01+time*3.14159)*0.1
	+ cos(uv.x*resolution.x*sin(uv.y*resolution.y*0.01+time)*uv.y*resolution.y*3200.0+time)*3.0*0.3;
	
	o.a = 1.0;
}

