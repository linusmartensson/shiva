#version 150

uniform sampler2D tex;
uniform float blur;
uniform vec2 resolution;
out vec4 color;
in vec2 uv;

uniform float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );


void main(void)
{
	color = vec4(0.0);
	color += texture2D( tex, uv, 0 ) * weight[0];
		
	color += texture2D(tex,uv+vec2(offset[1],0)/resolution) * weight[1];
	color += texture2D(tex,uv-vec2(offset[1],0)/resolution) * weight[1];
			
	color += texture2D(tex,uv+vec2(offset[2],0)/resolution) * weight[2];
	color += texture2D(tex,uv-vec2(offset[2],0)/resolution) * weight[2];
	
	color = blur*color + (1.0-blur)*texture2D( tex, uv);
}
