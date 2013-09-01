 #version 150
layout(points) in;
layout(points, max_vertices = 10) out;


in vec4 gposition[];
in vec4 gposition2[];
in vec4 gcolor[];
in vec4 gacc[];

out vec4 color;
out float distance;
out float sz;
out float dd;

void main() {
	float acc = length(gacc[0]);
	gl_Position = gposition[0];
	float d = gposition[0].z*0.001;
	const float msz = 40.0;
	sz = clamp(10.0+clamp(d*10.0,0.0,msz*0.9),1.0,msz);
	gl_PointSize = sz;
	distance = 1.0-sz/msz;
	dd = d;
	color.w = gcolor[0].a;
	color.rgb = gcolor[0].rgb*gcolor[0].a*(10.0/(gacc[0].w*gacc[0].w));
    EmitVertex();
	EndPrimitive();
}

