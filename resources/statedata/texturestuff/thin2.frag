#version 150

uniform sampler2DMS tex;
out vec4 color;
in vec2 uv;

void main(void)					
{
	color = vec4(0.0);
	
	float p1 = (texelFetch( tex, ivec2(gl_FragCoord.xy), 0 ).r);
	float p2 = (texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(0,1), 0 ).r);
	float p3 = (texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(1,1), 0 ).r);
	float p4 = (texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(1,0), 0 ).r);
	float p5 = (texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(1,-1), 0 ).r);
	float p6 = (texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(0,-1), 0 ).r);
	float p7 = (texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(-1,-1), 0 ).r);
	float p8 = (texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(-1,0), 0 ).r);
	float p9 = (texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(-1,1), 0 ).r);
	
	int n = int(round(p2+p3+p4+p5+p6+p7+p8+p9));
	int s = int(round(abs(p1-p2)+abs(p2-p3)+abs(p3-p4)+abs(p4-p5)+abs(p5-p6)+abs(p7-p8)+abs(p8-p9)+abs(p9-p2))/2.0);

	if(p1>0.5 && 2<=n && n<=6 && s==1 && int(p2+p4+p8)==0 && int(p2+p6+p8)==0 && p7>0.5){
		color = vec4(0.0,1.0,0.0,1.0);
	} else {
		color = texelFetch( tex, ivec2(gl_FragCoord.xy), 0 );
	}
}
