#version 330
in vec2 uv;
out vec4 texOut;
uniform sampler2D beta;
uniform sampler2D img;

void main(void)
{
	vec4 beta = texture2D(beta, vec2(0.0,1.0)+uv.xy*vec2(1.0,-1.0)).gbar; 
	vec4 j = texture2D(img, vec2(0.0,1.0)+uv.xy*vec2(1.0,-1.0)).gbar; 
	vec4 BS = beta;
	vec4 BB = 1.0-beta;
	texOut = vec4(BS);
	texOut.a = 1.0;
}

/*
L1 - L2 = B
L2 = 1 - B

L1 - (1-B) = B
L1 - 1 + B = B
L1 - 1 = 0
L1 = 1 ??????<

*/