 #version 150
layout(points) in;
layout(points, max_vertices = 10) out;


in vec4 gposition[];
in vec4 gposition2[];
in vec4 gcolor[];

out vec4 color;
out float distance;
out float sz;
out float dd;

void main() {
	
	gl_Position = gposition[0];
	float d = gposition[0].z;
	const float msz = 40.0;
	sz = clamp(4.0+clamp(d*0.025,0.0,msz*0.9),1.0,msz);
	sz = max(sz, 12.0-d*0.1);
	gl_PointSize = sz;
	distance = 1.0-sz/msz;
	dd = d;
	color = gcolor[0]*0.1;
	
    EmitVertex();
	EndPrimitive();
}
