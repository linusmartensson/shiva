
float4 compAcc(float4 p, float4 g){
	float4 gp;
	gp.x = g.x-p.x;
	gp.y = g.y-p.y;
	gp.z = g.z-p.z;
	float d = gp.x*gp.x + gp.y*gp.y + gp.z*gp.z + 1.0f;
	d = rsqrt(d);
	d = d*d*d;
	gp *= g.w*d;
	return gp;
}

#define SHARED_MEM_SIZE 512
#define MULT_FACTOR 8

__kernel void gravitymap(__global float4* pos, __global float4* acc, float time)
{
	time *= 3.0;
	unsigned int p = get_global_id(0);
	unsigned int sz = get_global_size(0);
	unsigned int l = get_local_id(0);
	float4 pp = pos[p];
	float4 sum = (float4)(0,0,0,0);
	
	__local float4 shared[SHARED_MEM_SIZE];
	unsigned int inc = get_local_size(0)*MULT_FACTOR>=SHARED_MEM_SIZE?SHARED_MEM_SIZE:get_local_size(0)*MULT_FACTOR;
	
	for(int i=0;i<sz;i+=inc){
	
		barrier(CLK_LOCAL_MEM_FENCE);
		if(l*MULT_FACTOR < inc){
			for(int j=0;j<MULT_FACTOR;j++){
				shared[l*MULT_FACTOR+j] = pos[i+l*MULT_FACTOR+j];
			}
		}
		barrier(CLK_LOCAL_MEM_FENCE);
		
		for(int j=0;j+i<sz&&j<inc;j++){
			sum +=	compAcc(pp, shared[j]);
		}
    }
	sum *= (float4)(1.0,1.0,1.0,0.0);
	float4 a = acc[p] + sum*time;
	pp += a*time;
	barrier(CLK_GLOBAL_MEM_FENCE);
	acc[p] = a;
	pos[p] = pp;
}