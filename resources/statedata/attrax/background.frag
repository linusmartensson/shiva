
#version 330


in vec2 uv;
out vec4 c;
uniform float time;

void main(void)
{
	float tt = pow(time*0.2,16.0);

	float y = tt/(tt+1.0)*0.8;
	float x = 0.35;
	float k = 0.81-y;
	c = vec4(0.2,0.0,0.0,1.0);
	

	if(uv.y < (cos(uv.x-x)-1.0)*k+y+0.05){
		c.g = 2.0*cos(time*3.0+sin(uv.x*32.0+y)*0.3+cos(uv.y*2.0-y)*16.0)*0.1+0.5;
	} else {
		c.g = step(cos(atan(uv.x-x,uv.y-y)*30.0+time*10.0)*2.0,0.0)*2.0-0.67;
		c.r += (2.0-sqrt((uv.x-x)*(uv.x-x)+ (uv.y-y)*(uv.y-y)))*0.4+abs(c.g*0.6)*10.0;
		c.rg += time/(time+1.0)*20.0;
	}
	c.r *= uv.y;
	c.rg += max(0.0,200.0*(0.03-abs(uv.y - ((cos(uv.x-x)-1.0)*k+y+0.05)))-(1.05-abs(-cos(uv.x-x)))*64.0+(time*0.1/(time*0.1+1.0))*5.0);

	c.rg *= time*0.1/(time*0.1+1.0)*1.1-0.1;
	c.r *= 1.0+sin(uv.y*3514.0+uv.x*3223.1*sin((gl_FragCoord.y*3232543.1+time)*2356.234+sin(gl_FragCoord.x*34521.0+time)*103200.0)*10.1+time*3451.0*(uv.x*24.98+uv.y*3452.0+10.0))*0.3;
}
