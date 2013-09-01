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

uniform float slomoon;
uniform float slomooff;

void main(void) {
	vec4 vpos = vec4(position,1.0)+vec4(offset,0.0,0.0,0.0);

	float x = vpos.x+offset2*50.0;
	float y = vpos.y;
	

	float tt = 0.0;
	
	tt = clamp(smoothstep(slomoon-1.0, slomoon, time),0.0,1.0) * (slomoon + (slomoon - time)*0.1) 
   *(1.0-clamp(smoothstep(slomooff-5.0, slomooff, time),0.0,1.0)) +
	    +(1.0-clamp(smoothstep(slomoon-1.0, slomoon, time),0.0,1.0))*time
		+clamp(smoothstep(slomooff-5.0, slomooff, time),0.0,1.0)*(slomooff - (slomooff-slomoon)*0.9+(time-slomooff)*2.0);
		
	float z = ((1000.0-gl_VertexID)+floor((tt-on)*860.0)*2.0)*0.3-8000.0;

	y-=sin(z+offset)*30.0;
	float r = sqrt(x*x+y*y+z*z);
	float inc = acos(z/r);
	float azim = atan(y,x);
		azim += cos(z*1.0)+sin(z*20.0)*20.0+z*x*30.0;
	
	x = r*sin(inc)*cos(azim);
	y = r*sin(inc)*sin(azim);
	z = r*cos(inc);
	rpos = vec4(x,y,z,1.0);
		

	y+=200.0+sin(z*0.004)*100.0*sin(z*0.0001+offset)+sin(z*0.0003+offset)*300.0;
	y *= 0.5;
	
	z+=sin(tt*0.0001)*5000.0;
	x+=sin(tt*0.0002)*5000.0;
	y+=sin(tt*0.0003)*5000.0;
	
	r = sqrt(x*x+y*y+z*z);
	inc = acos(z/r);
	azim = atan(y,x);

	azim += z*0.0001+sin(z*0.001)*z*0.001+(offset*3.14159);

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

	azim += z*0.004;

	x = r*sin(inc)*cos(azim);
	
	y = r*sin(inc)*sin(azim);
	z = r*cos(inc);
	x-=200.0;
	
		z -= sin(z*0.001*offset+offset)*500.0*(offset+3.0);

	vpos = vec4(x,y,z,1.0);
	opos = vpos;
	vid = gl_VertexID;
}