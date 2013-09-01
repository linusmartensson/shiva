#version 330

in vec3 position;
out vec4 opos;
out float vid;
out vec4 rpos;
uniform mat4 camera;
uniform float time;

void main(void) {
	vec4 vpos = vec4(position,1.0);

	vpos.x = vpos.x+sin(time*0.03*sin(gl_VertexID+0.0))*sin(gl_VertexID)*4.0*sin(time*0.3+gl_VertexID+vpos.y+vpos.z)*10.0;
	vpos.y = vpos.y+sin(time*0.04*sin(gl_VertexID+1.0))*sin(gl_VertexID+3.0)*4.0*sin(time*0.1+gl_VertexID+vpos.y+vpos.z)*10.0;
	vpos.z = vpos.z+sin(time*0.01*sin(gl_VertexID+2.0))*sin(gl_VertexID+8.0)*4.0*sin(time*0.3+gl_VertexID+vpos.y+vpos.z)*10.0;
	
	rpos = vpos;

	opos = (vpos)*camera;
	vid = gl_VertexID;
}