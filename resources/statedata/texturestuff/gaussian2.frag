#version 150

uniform sampler2DMS tex;
uniform vec2 res;
out vec4 color;

in vec2 uv;

uniform float weight[8] = float[]( 0.3989422804, 0.2419707245, 0.0539909665, 0.0044318481, 0.0001338302, 0.0000014867, 0.0000000060, 0.0000000000);
void main(void)					
{
	color = vec4(0.0);
	
	color -= texelFetch( tex, ivec2(gl_FragCoord.xy), 0 );
	color += texelFetch( tex, ivec2(gl_FragCoord.xy), 0 ) * weight[0];
		
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,1).yx, 0) * weight[1];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,1).yx, 0) * weight[1];
				
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,2).yx, 0) * weight[2];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,2).yx, 0) * weight[2];
			
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,3).yx, 0) * weight[3];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,3).yx, 0) * weight[3];
			
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,4).yx, 0) * weight[4];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,4).yx, 0) * weight[4];

	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,5).yx, 0) * weight[5];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,5).yx, 0) * weight[5];
	
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,6).yx, 0) * weight[6];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,6).yx, 0) * weight[6];
	
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)+ivec2(0,7).yx, 0) * weight[7];
	color += texelFetch(tex,ivec2(gl_FragCoord.xy)-ivec2(0,7).yx, 0) * weight[7];
	
	
	color.rgba = abs(vec4(dot(color, vec4(0.25))));

	if(gl_FragCoord.x+7 >= res.x || gl_FragCoord.x-7 < 0) color.rgba = vec4(0.0);
}
