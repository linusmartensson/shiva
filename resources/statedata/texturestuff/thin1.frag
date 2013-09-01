#version 150

uniform sampler2DMS tex;
out vec4 color;
in vec2 uv;

void main(void)					
{
	color = vec4(0.0);
	
	int p1 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy), 0 ).r));
	int p2 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(0,1), 0 ).r));
//	int p3 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(0,2), 0 ).r));
	int p4 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(1,0), 0 ).r));
//	int p5 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(2,0), 0 ).r));
	int p6 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(0,-1), 0 ).r));
//	int p7 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(0,-2), 0 ).r));
	int p8 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(-1,0), 0 ).r));	
//	int p9 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(-2,0), 0 ).r));
	
	int p3 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(1,1), 0 ).r));
	int p5 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(1,-1), 0 ).r));
	int p7 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(-1,-1), 0 ).r));
	int p9 = int(round(texelFetch( tex, ivec2(gl_FragCoord.xy)+ivec2(-1,1), 0 ).r));

	int n = p2+p3+p4+p5+p6+p7+p8+p9;
	int s = (abs(p1-p2)+abs(p2-p3)+abs(p3-p4)+abs(p4-p5)+abs(p5-p6)+abs(p7-p8)+abs(p8-p9)+abs(p9-p2))/2;

	if(n == 1 || n==2 || n == 7 || n == 8 || s>=2){
		color = texelFetch( tex, ivec2(gl_FragCoord.xy), 0 );
		return;
	} else if(p1 == 1) {
		color = vec4(0.0,0.0,0.0,1.0);
	} else {
		color = texelFetch( tex, ivec2(gl_FragCoord.xy), 0 );
	}
}
