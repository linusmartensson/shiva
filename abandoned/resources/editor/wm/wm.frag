
#version 150

uniform sampler2D surfaceTexture;
uniform float active;

out vec4 out_Color;
in vec2 uv;

void main(void)
{
	vec2 uu = uv*-2.0+1.0;
	uu*=uu; 
	uu*=uu;
	uu*=uu;
	uu*=uu;
	uu*=uu;
	uu*=uu;
	float dist = clamp((pow(uu.x+uu.y, 1/64.0)-0.95)*5.0,0.0,0.2);
	out_Color = texture2D(surfaceTexture, uv)*(1.0-dist*3.0) + vec4(vec3(dist*(active*(0.2-dist)*40.0+0.5)),1.0);
}
