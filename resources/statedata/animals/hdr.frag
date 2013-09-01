
#version 150

out vec4 texOut;
in vec2 uv;
uniform sampler2DMS tex;

void main(void)
{
	vec3 g=vec3(0.0);
	for(int i=0;i<8;i++){
		g += texelFetch(tex, ivec2(gl_FragCoord.xy), i).rgb;
	}
	g *= 0.125;
	
	vec3 b=vec3(0.0);
	b += g*8.0;
	b += texelFetch(tex, ivec2(gl_FragCoord.xy)+ivec2( 1,-1), 0).rgb;
	b += texelFetch(tex, ivec2(gl_FragCoord.xy)+ivec2(-1, 1), 0).rgb;
	b += texelFetch(tex, ivec2(gl_FragCoord.xy)+ivec2( 1, 1), 0).rgb;
	b += texelFetch(tex, ivec2(gl_FragCoord.xy)+ivec2(-1,-1), 0).rgb;
	b += texelFetch(tex, ivec2(gl_FragCoord.xy)+ivec2( 1, 0), 0).rgb;
	b += texelFetch(tex, ivec2(gl_FragCoord.xy)+ivec2(-1, 0), 0).rgb;
	b += texelFetch(tex, ivec2(gl_FragCoord.xy)+ivec2( 0, 1), 0).rgb;
	b += texelFetch(tex, ivec2(gl_FragCoord.xy)+ivec2( 0,-1), 0).rgb;
	
	float exposure = 1.0;
	float maxbright = 10.0;
	g = g*exposure * (exposure / (maxbright + 1.0)) / (exposure + 1.0);

	
	//texOut = vec4(pow(g, vec3(1.0/2.2)), 1.0);
	texOut = vec4(g, 1.0);
}
