#version 150
layout(points) in;
layout(triangle_strip, max_vertices = 10) out;

out vec2 uv;
uniform mat4 m;

void main() {
	
	vec4 scale = vec4(vec3(200.0),1.0);

	gl_Position = vec4(-1.0,-1.0,0.0,1.0)*scale*m;
	uv = vec2(0.0,0.0);
    EmitVertex();
	
	gl_Position = vec4(-1.0,1.0,0.0,1.0)*scale*m;
    uv = vec2(0.0,1.0);
	EmitVertex();
	
	gl_Position = vec4(1.0,-1.0,0.0,1.0)*scale*m;
    uv = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = vec4(1.0,1.0,0.0,1.0)*scale*m;
    uv = vec2(1.0,1.0);
	EmitVertex();
	
	EndPrimitive();
}
