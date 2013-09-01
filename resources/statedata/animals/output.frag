
#version 330
out vec4 o;
in vec2 uv;
uniform sampler2D w;
uniform sampler2D a;
uniform sampler2D c;
uniform float time;
uniform float vis;
uniform float range;

void main(void)
{
	
	o = texture2D(c,uv);

	if(distance(texture2D(w,uv).xyz,vec3(0.0)) > range) o = vec4(0.0,0.0,0.0,1.0);
	o.a = 1.0;
	
	o.rgb = o.rgb*0.0+vec3(0.5,2.5,5.7)*(2.0-uv.y)*0.01;
}

