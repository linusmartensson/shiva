#version 150
layout(points) in;
layout(line_strip, max_vertices = 10) out;


in vec4 gposition[];
in vec4 gposition2[];
in vec4 gcolor[];

out vec4 color;

void main() {
	
	float len = length(gposition[0]-gposition2[0]);

	gl_Position = gposition[0];
	color = gcolor[0]*clamp(len,0.0,1.0);
    EmitVertex();

	gl_Position = gposition2[0];
	color = vec4(0.0) ;
    EmitVertex();
	EndPrimitive();
}
