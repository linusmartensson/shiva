
#version 150

out vec4 out_Color;
in vec2 uv;
uniform sampler2D text;
uniform float offset;
uniform float time;
uniform float names;


float tf(float c){
	const float beatdist = 1000.f*60/140.f;

	return (c-1.f)*beatdist*4.f;
}


float toff(float tt, float off){
	return max(tt-tf(off),0.0);
}

float timefac(float tt, float off, float spd){
	return clamp(toff(tt, off)*spd,0.0,1.0);	
}

void main(void)
{	
	float t = timefac(time, 81.0, 0.00005);
	vec2 p = vec2(uv.x*4.0-3.0*0.975,1.-mod(uv.y+offset*0.8+time*0.001,names*0.8));
	vec4 g = texture(text, p);
	out_Color = g*p.y*8.0;
	out_Color.b*=0.05;
	out_Color.r*=5.0;
	out_Color.g*=p.y;

	out_Color += uv.x<0.7?0.0:0.4*
	sin(mod((1.0+uv.y)*4532.0,sin(time*531+(uv.x+1.0)*4112.0)*100+200)*(2.0+mod(time*3251.0+uv.x*3215.8,sin(4532.0*(uv.y+1.0)*time+1.0)+3.0)))
	*sin(mod((1.0+uv.x)*4132.0,sin(time*321+(uv.y+1.0)*4112.0)*100+200)*(2.0+mod(time*3251.0+uv.y*2135.0,sin(4532.0*(uv.x+1.0)*time+1.0)+3.0)))
	;
	out_Color *= t;
}
