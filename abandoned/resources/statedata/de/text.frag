
#version 330
out vec4 c;
in vec2 uv;

uniform sampler2D text;
void main(void)
{
	c = texture2D(text, uv-0.5);
	if(c.a > 0.5){
		c = vec4(20.0,20.0,80.0,1.0);
	} else {
		c = vec4(0.0);
	}
}