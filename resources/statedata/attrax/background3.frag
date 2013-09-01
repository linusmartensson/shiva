#version 330

in vec2 uv;
out vec4 c;
uniform float time;
uniform float on;
uniform sampler2D ewerk;
uniform sampler2D beer;

void main(){
	c = vec4(1.0);

	float x = uv.x*2.0-1.0;
	float y = uv.y*2.0-1.0;

	x+=0.9;
	y+=1.3;

	float r = atan(x,y);
	float d = sqrt(x*x+y*y);
	

	c.rg *= step(min(sin(r*20+time*7.0),d-0.05),0.0)*2.0;
	c.rg += d*0.1;
	
	
	vec2 uu = uv*2.0-1.0;
	uu = uu*0.5+0.5;

	c += texture2D(ewerk, vec2(0.0,1.0)+uu*vec2(1.0,-1.0)).a*(1.0-texture2D(ewerk, vec2(0.0,1.0)+uu*vec2(1.0,-1.0)).b);
	
	vec4 t = texture2D(beer, vec2(0.0,1.0)+uv*vec2(1.0,-1.0))*clamp(smoothstep(on+5.0,on+10.0,time),0.0,1.0);

	c = mix(c, t, vec4(t.a*0.75));
	c *= clamp(smoothstep(on,on+5,time),0.0,1.0);
	

}