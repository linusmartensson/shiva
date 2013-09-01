#version 150
layout(points) in;
layout(points, max_vertices = 10) out;

in vec4 geopos[];
in vec4 opos[];
out vec4 pos;
out vec4 gpos;
uniform float distort;
uniform mat4 mat;
in int id[];
float sig(float t){return 1.f/(1.f+pow(2.71828f,-t)); }
void main() {
	vec4 m = vec4(opos[0].xyz, 0.f);
	vec4 n = vec4(geopos[0].xyz, 0.f);
	vec4 d = (mat*vec4(m.x,distort*0.f,m.z,1.f));
	vec4 dd = (mat*(vec4(m.xyz,1.f)*vec4(vec2(1.f-min(distort*0.1,1.5f),1).yxy,1.f)));
	gl_Position = geopos[0];
	
	pos = opos[0];
	gpos = geopos[0];
	gl_PointSize = min(max(1.0f, (100-geopos[0].z*0.02)*0.1f),64.0f);
	EmitVertex();
	EndPrimitive();
}
