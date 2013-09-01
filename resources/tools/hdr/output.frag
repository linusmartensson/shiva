
#version 150

out vec4 texOut;
in vec2 uv;
uniform sampler2DMS tex;
uniform float exposure;
uniform float maxbright;

void main(void)
{
	vec3 g=vec3(0.0);
		g = texelFetch(tex, ivec2(gl_FragCoord.xy), 0).rgb;
	texOut = vec4(pow(g, vec3(1.0/2.2)), 1.0);
}
