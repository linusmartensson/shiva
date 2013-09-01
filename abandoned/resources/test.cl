__kernel void gravitymap(__global float4* pos, __global float4* acc, float time)
{
	int i = get_global_id(0);
	float4 p = pos[i];
	float4 a = acc[i];

	if(p.x > 1.0 || p.x < -1.0 || p.y > 1.0 || p.y < -1.0) p = (float4)(randomvalue(i, 11422, time)*2.0-1.0, randomvalue(i,213542,time)*2.0-1.0,1.0,1.0);

	p += a*0.01;

	pos[i] = p;
	acc[i] = a;
}