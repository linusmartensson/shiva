
#version 330

uniform float time;
in vec2 uv;
out vec4 c;

void main(void)
{
	c = vec4(0.0);
	float tt = pow(time*0.2,16.0);

	float y = tt/(tt+1.0)*0.8;
	float yy=y-0.9;
	float x = 0.35;
	float k = 0.81-y;
	c = vec4(0.2,0.0,0.0,1.0);
	
	float ao = 0.5;

	float f = (cos(uv.x-x)-1.0)*k+y+0.05;

	float ux = uv.x-uv.y*0.2;

	c.r += step(abs(ux*2.0-ao)*5.0/max(f-uv.y,0.0),0.1)*10.0*step(sin(uv.y*32.0/max(f-uv.y,0.0)+time*320),0.0);
	c.r += step(abs(ux*2.0-ao+0.6*(1.0-uv.y/0.85))*5.0/max(f-uv.y,0.0),0.1)*10.0;
	c.r += step(abs(ux*2.0-ao-0.6*(1.0-uv.y/0.85))*5.0/max(f-uv.y,0.0),0.1)*10.0;
	c.r *= y;
	c.r *= clamp(0.85-uv.y,0.0,1.0);
}