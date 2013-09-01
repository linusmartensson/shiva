#version 330

in vec3 position;
out vec4 opos;
uniform float offset;
out float vid;
out vec4 rpos;
uniform mat4 camera;
uniform float time;
uniform float mark;
uniform float offset2;

uniform sampler2D spectrum;
uniform float count;

void main(void) {
	vec4 vpos = vec4(position,1.0)+vec4(offset,0.0,0.0,0.0);

	float x = vpos.x+offset2*50.0;
	float y = vpos.y;
	float z = vpos.z;
	y-=10.0;
	if(mark>0.5) {
		y-=00.0;
		x+=10*offset;
	}
	float r = sqrt(x*x+y*y+z*z);
	float inc = acos(z/r);
	float azim = atan(y,x);
	if(mark>0.5) {
		azim += (z*1.0)+sin(z+time);
	}
	x = r*sin(inc)*cos(azim);
	y = r*sin(inc)*sin(azim);
	z = r*cos(inc);
	rpos = vec4(x,y,z,1.0);
	if(mark>0.5){
		if(y>-20.0){
			y=-20.0;
			x+=sin(time*0.21)*30.0;
			z+=sin(time*0.73)*30.0;
		}
		
	 }

	if(mark>0.5){
		y+=10.0;
	}
	if(mark > 0.5){
		z+=mod(gl_VertexID,3.0)*40.0;
	}
	r = sqrt(x*x+y*y+z*z);
	inc = acos(y/r);
	azim = atan(z,x);
	
	azim += sin(z*0.002)*0.1;
	inc += sin((z*0.001))*0.3;

	x = r*sin(inc)*cos(azim);
	z = r*sin(inc)*sin(azim);
	y = r*cos(inc);

	float a = texture2D(spectrum, vec2(mod(gl_VertexID/count*20,0.2)*sin(z*0.01)*0.01,0.5)).r*100.0;

	x += offset*a*sin(z*0.1);
	y += a*cos(z*0.1)*10.0;




	vpos = vec4(x,y,z,1.0);
	opos = (vpos)*camera;
	vid = gl_VertexID;
}