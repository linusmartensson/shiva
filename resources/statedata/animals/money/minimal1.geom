#version 150
layout(points) in;
layout(line_strip, max_vertices = 10) out;


in vec4 gposition[];
in vec4 gposition2[];
in vec4 gcolor[];
in vec4 gacc[];

out vec4 color;
out float pos;

void main() {
	
	float len = length(gposition[0]-gposition2[0]);
	float acc = length(gacc[0]);
	
	color.w = gcolor[0].a*acc/(length(gposition[0])*0.1+1000.0);//*clamp(len*0.1,0.0,1.0);
	color.rgb = gcolor[0].rgb*color.w;
	
	gl_Position = gposition[0];
	pos = 0.0;
	EmitVertex();

	gl_Position = gposition2[0];
	pos = 1.0;
	EmitVertex();
	EndPrimitive();
}
