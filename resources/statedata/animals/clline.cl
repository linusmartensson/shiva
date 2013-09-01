__kernel void clconway(__global float4* position, float time, __global float4 *astate, __global float4* temp){
			
	unsigned int globalId = get_global_id(0);
	
	int lim = sqrt(40000.f);

	float4 p = position[globalId];

	float4 px = (float4)(
		fmod(globalId,lim*1.f)-lim/2,
		-10.f,
		(globalId/lim)-lim*0.5,
	1.0f)*20.0f;
	p=px;;

	float t = 0.f;
			
	p.w = 1.0f;
	position[globalId] = p;
}

__kernel void clzero(__global float4* value, float x, float y, float z, float w){
	value[get_global_id(0)] = (float4)(x,y,z,w);
}

float ra(float id, float k){
	return fmod(fmod(id*432615+325,413532)*214341+fmod(k*3215+101,41531)*433251, 23521.f)/23521.f;
}

__kernel void clboom(__global float4* p, float time, float k, float i){
	int f = get_global_id(0);
	p[f] = (float4)(ra(f+time,0+k), ra(f+time,1+k), ra(f+time,2+k), ra(f+time,3+k))*0.901f-i;

}