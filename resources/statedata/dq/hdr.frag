#version 330

uniform sampler2DMS tex;
uniform sampler2DMS greetz;
in vec2 uv;
uniform float time;
out vec4 texOut;
uniform vec2 resolution;

void main(void)
{
	vec2 u = uv*2.0-1.0;

	vec3 gz = texelFetch(greetz, ivec2(-30, -5)+ivec2((vec2(0.0,1.0) + uv*vec2(1.0,-1.0))*266.0), 0).rgb;

	float off = clamp(pow(pow(abs(u.x*0.9),4.0)+pow(abs(u.y*0.9),4.0),1.0/1.0),0.0,1.0);

	vec3 g = vec3(0.0); 
	for(int i=0;i<8;i++){
		g += texelFetch(tex, ivec2(gl_FragCoord.xy-(gl_FragCoord.xy-resolution/2.0)*off*0.2), i).rgb;
	}

	g*=1.1-length(u);
	
	float k2 = 1.0-clamp(pow(distance(time,61.5),1.0/10.0), 0.0,1.0);
	g.rgb = vec3(1.0)*k2 + g.rgb*(1.0-k2); 
	
	k2 = 1.0-clamp(pow(distance(time,67.5),1.0/10.0), 0.0,1.0);
	g.rgb = vec3(1.0)*k2 + g.rgb*(1.0-k2); 
	
	k2 = 1.0-clamp(pow(distance(time,73),1.0/10.0), 0.0,1.0);
	g.rgb = vec3(1.0)*k2 + g.rgb*(1.0-k2); 

	k2 = 1.0-clamp(pow(distance(time,79.5),1.0/6.0), 0.0,1.0);
	g.rgb = vec3(1.0)*k2 + g.rgb*(1.0-k2); 

	g.rgb += 1.2-clamp(pow(vec3(sin(uv.y*resolution.x*0.5-time*2.0)*0.5), vec3(1.0/2.2)),0.0,1.0);
	g.rgb += 0.3-clamp(pow(vec3(sin(uv.y*resolution.x*0.01*(1.0+cos(time*0.01)*0.1)+sin(time*0.1)*0.3+time*0.04)*0.3), vec3(1.0/1.2)),0.0,1.0);	
	g.gb*=vec2(2.0,1.3);
	
	float k = 1.0-clamp(pow(distance(time,42.149),1.0/10.0), 0.0,1.0);

	g.rgb=vec3(10.0*(sin(time*3215432*uv.x+uv.y*time*3124513)+sin(time*32131*sin(uv.y*32143*time+uv.x*1231532*time))))*k+g.rgb*(1.0-k);
	
	k = clamp(time-115,0.0,1000.0);
	g.rgb=vec3(10.0*(sin(time*3215432*uv.x+uv.y*time*3124513)+sin(time*32131*sin(uv.y*32143*time+uv.x*1231532*time))))*k+g.rgb*(1.0-k)-k*k*4.0;

	g.rgb *= gz*2.0*clamp(time-105.0,0.0,10.0)*0.1 + (1.0-clamp(time-105.0,0.0,10.0)*0.1);

	texOut.xyz = g/4.0*clamp(4.0-pow(pow(abs(u.x),8.0)*0.8+pow(abs(u.y),8.0)*0.8,1.0/8.0)*4.0,0.0,1.0);

	texOut.a = 1.0;
}