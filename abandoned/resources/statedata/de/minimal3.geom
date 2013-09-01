#version 330
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in vec4 gposition[];
in vec4 pp[];
out vec4 ppo;

void main() {
	gl_Position = gposition[0];
	ppo = pp[0];
    EmitVertex();
	gl_Position = gposition[1];
	ppo = pp[1];
    EmitVertex();
	gl_Position = gposition[2];
	ppo = pp[2];
    EmitVertex();
	EndPrimitive();
}
