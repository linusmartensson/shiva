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

uniform float on;
uniform sampler2D wform;

uniform float slomoon;
uniform float slomooff;

void main(void) {
	vec4 vpos = vec4(position,1.0)+vec4(offset,0.0,0.0,0.0);

	float x = vpos.x+offset2*50.0;
	float y = vpos.y;
	

	float tt = 0.0;
	
	tt = time*0.1;
		
	float z = (1000.0-gl_VertexID)+8000 + time*100.0;

	y-=sin(z+offset)*10.0;
	float r = sqrt(x*x+y*y+z*z);
	float inc = acos(z/r);
	float azim = atan(y,x);
	//	azim += cos(z*1.0)+sin(z*20.0)*20.0+z*x*30.0;
	
	x = r*sin(inc)*cos(azim);
	y = r*sin(inc)*sin(azim);
	z = r*cos(inc);
	rpos = vec4(x,y,z,1.0);
		

	y+=200.0+texture2D(wform, vec2(abs(sin(gl_VertexID/2000.0)/25.0), 0.5)).r*10000.0+sin(z*0.004)*100.0*sin(z*0.0001+offset)+sin(z*0.0003+offset)*300.0;
	y *= 2.5;
	
//	z+=sin(tt*0.0001)*5000.0;
//	x+=sin(tt*0.0002)*5000.0;
//	y+=sin(tt*0.0003)*5000.0;
	
	r = sqrt(x*x+y*y+z*z);
	inc = acos(z/r);
	azim = atan(y,x);

//	azim += z*0.0001+sin(z*0.001)*z*0.001+(offset*3.14159);

	x = r*sin(inc)*cos(azim);
	y = r*sin(inc)*sin(azim);
	z = r*cos(inc);
	
	z+=sin(z*0.3)*20.0;
	y -= sin(z*0.0002+offset*0.3)*400.0;
	
		y*=offset;

	z += cos(z*0.0001)*200.0+tt*200.0;
	r = sqrt(x*x+y*y+z*z);
	inc = acos(z/r);
	azim = atan(y,x);

	azim += z*0.004*sin(time*0.01)*2.0;

	x = r*sin(inc)*cos(azim);
	
	y = r*sin(inc)*sin(azim);
	z = r*cos(inc);
	x-=200.0;
	
		z -= sin(z*0.001*offset+offset)*500.0*(offset+3.0);

	vpos = vec4(z,y,x+1500,1.0);
	opos = vpos;
	vid = gl_VertexID;
}