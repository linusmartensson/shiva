#version 330

in vec2 uv;
out vec4 c;
uniform float on;
uniform float off;
uniform float time;
uniform sampler2D text6;


void main(){
	c.rgba = texture2D(text6, vec2(0.0,1.5)+(uv*vec2(1.0,-1.0))*vec2(1.0,2.0)).aaaa*(0.4*abs(sin(time*0.3+uv.x*9.0))+0.2);
	c.rgba += texture2D(text6, vec2(uv.x, 0.3)).aaaa*(0.5-abs(uv.y-0.1-0.5))*(0.1+clamp(time*0.3,0.0,0.4)+sin(time*4.0+uv.x*7.0+sin(time*1.1)*4.0)*uv.x*0.2);
	c.rgba += texture2D(text6, vec2(uv.x, 0.9)).aaaa*(0.5-abs(uv.y+0.2-0.5))*(0.1+clamp(time*0.3,0.0,0.4)+sin(time*4.0+uv.x*7.0+sin(time*0.9)*4.0)*uv.x*0.2);
	c.rgba = clamp(c.rgba, vec4(0.0), vec4(0.5));

	c.rgba *= clamp(time-on, 0.0, 1.0);
	c.rgba *= clamp(off-time, 0.0, 1.0);

	if(c.r < abs(0.05)) discard;
}