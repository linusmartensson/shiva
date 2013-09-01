#version 150

uniform sampler2DMS dx;
uniform sampler2DMS dy;
out vec4 color;
in vec2 uv;

void main(void)					
{
	color = vec4(0.0);
	
	float x = texelFetch( dx, ivec2(gl_FragCoord.xy), 0 ).r;
	float y = texelFetch( dy, ivec2(gl_FragCoord.xy), 0 ).r;
	float dir = atan(x, y);
	float m = sqrt(x*x + y*y);
	
	float alpha = 0.5/sin(3.14159265/8.0);

	vec2 offset = round(vec2(x,y)*alpha/m);

	float fwdx = texelFetch(dx, ivec2(gl_FragCoord.xy + offset), 0).r;
	float fwdy = texelFetch(dx, ivec2(gl_FragCoord.xy + offset), 0).r;
	float fdir = atan(fwdx, fwdy);
	float fm = sqrt(fwdx*fwdx + fwdy*fwdy);
	


	float backx = texelFetch(dx, ivec2(gl_FragCoord.xy - offset), 0).r;
	float backy = texelFetch(dx, ivec2(gl_FragCoord.xy - offset), 0).r;
	float bdir = atan(backx, backy);
	float bm = sqrt(backx*backx + backy*backy);
	
	float thresh = 0.02;

	if(fm > m || bm > m || m < thresh){
		color = vec4(0.0);
	} else {
		color = vec4(0.9);
	}
	
}
