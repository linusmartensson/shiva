
#version 150

out vec3 c;
in vec4 pos;
in vec4 gpos;

uniform float distort;

void main(void)
{
	vec2 pc = (gl_PointCoord.xy-vec2(0.5,0.5))*2.f;
	vec3 py = max(vec3(0.f), 105.f-abs(pos.yyy));
	vec3 m = pos.yyy*2.0;
	c = vec3(1+pc.x+gpos.z*0.0005-max(0.0,1.0-gpos.z*0.0001),max(0.0,0.2+pc.x*0.3+gpos.z*0.0001),0.05+max(0.0,max(0.0,2.0-gpos.z*0.0007))-(0.1-dot(pc,pc)*0.1))*3.0;
	c = pow(c,vec3(2.0));
	c = c.rgb;
}
