#version 150
layout(points) in;
layout(points, max_vertices = 10) out;

in vec4 geo_Position[];
in float z[];
in vec4 acc[];
out vec4 accv;
out float size;
out vec4 pos;

void main() {
	gl_Position = geo_Position[0];
	gl_PointSize = 1.0;
	size = z[0]+0.1;
    accv = acc[0];
    EmitVertex();

	EndPrimitive();
}
