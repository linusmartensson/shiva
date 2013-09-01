#version 330

in vec2 uv;
out vec4 c;
uniform float time;
uniform float on;

void main(){
	c = vec4(0.0,100.0/(1.0+time-on),0.0,1.0);

	c.r += uv.y*10.0* (cos(uv.x*2.0-1.0))/(1.0+time-on)*1.0;
	c.r *= 1.0+sin(uv.y*3514.0+uv.x*3223.1*sin((gl_FragCoord.y*3232543.1+time)*2356.234+sin(gl_FragCoord.x*34521.0+time)*103200.0)*10.1+time*3451.0*(uv.x*24.98+uv.y*3452.0+10.0))*0.3;
}