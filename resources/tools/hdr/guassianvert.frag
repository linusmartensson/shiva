#version 150

uniform sampler2DMS tex;
uniform float blur;
uniform vec2 resolution;
out vec4 color;
in vec2 uv;

uniform float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );
void main(void)
{
	color = vec4(0.0);
	
	color += texelFetch( tex, ivec2(gl_FragCoord.xy), 0 ) * weight[0];
		
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,1), 0) * weight[1];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,1), 0) * weight[1];
				
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,2), 0) * weight[2];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,2), 0) * weight[2];
			
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,3), 0) * weight[3];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,3), 0) * weight[3];
			
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,4), 0) * weight[4];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,4), 0) * weight[4];

	color = blur*color + (1.0-blur)*texelFetch( tex, ivec2(gl_FragCoord.xy), 0 );
	
}
