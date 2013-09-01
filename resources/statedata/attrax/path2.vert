#version 330

in vec3 position;
out vec4 opos;
uniform float offset;
out float vid;
out float mup;
out vec4 rpos;
uniform mat4 camera;
uniform float time;
uniform float mark;
uniform float count;

uniform sampler2D wform;
uniform sampler2D spectrum;

void main(void) {
	vec4 vpos = vec4(position+vec3(0.0,0.0,0.0),1.0);

	

	float x = vpos.x;
	float y = vpos.y;
	float z = vpos.z;
	
	y-=10.0;
	float r = sqrt(x*x+y*y+z*z);
	float inc = acos(z/r);
	float azim = atan(y,x);

	x = r*sin(inc)*cos(azim);
	y = r*sin(inc)*sin(azim);
	z = r*cos(inc);
	
	z += (gl_VertexID%2)*200.0;
	y += (gl_VertexID%2)*50.0;
	x += (gl_VertexID%2)*100.0;
	z += 40000.0;
	
	z*=0.1;
	y += texture2D(spectrum, vec2(gl_VertexID/count*0.2,0.5)).r*10000.0;
	y*=sign(offset);

	vpos = vec4(x,y,z,1.0);
	opos = (vpos)*camera;
	vid = gl_VertexID;
}