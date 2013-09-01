#version 330

in vec2 uv;
out vec4 c;
uniform float on;
uniform float off;
uniform float time;
uniform sampler2D greetz;
float mr(float x, float y, float t, float fac){
	float r = atan(x, y);
	float d = sqrt(x*x+y*y);
	
	r+=t;
	d*=fac;

	x = sin(r)*d;

	return abs(x);

}

void main(){
	c = vec4(1.0);
	float x= uv.x*2.0-1.0;
	float y= uv.y*2.0-1.0;
	
	float t = max(time - on,0.0)*10.0;
	float t2 = max(time- (off-5.0),0.0)*1.0;

	x-=t2;

	c.r = step(mr(x+0.3,y-sin(t/(t+2.0))*0.3, t/(t+1.0)+(t*0.21)/(t*0.21+1.0), 0.1),0.02)*50.0;
	c.r += step(mr(x-0.3,y-sin(t/(t+2.0))*0.2, t/(t+1.0)+(t*0.1)/(t*0.1+1.0)*0.8, 0.1),0.04)*50.0;
	c.r *= min(t*0.1,1.0);
	c.rgba = texture2D(greetz, vec2(0.0,1.0)+uv*vec2(1.0,-1.0)).aaaa;
	c *= 1.0-clamp(smoothstep(off-5.0,off,time), 0.0,1.0);
}