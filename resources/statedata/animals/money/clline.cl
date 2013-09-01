__kernel void clconway(__global float4* position, float time, __global float4 *astate, __global float4* temp){
			
	unsigned int globalId = get_global_id(0);
	
	int lim = sqrt(40000.f);

	float4 p = position[globalId];

	float4 px = (float4)(
		0.0,
		-1.0,
		globalId*0.02,
	1.0f)*20.0f;
	p=px;;

	
	float4 s = astate[globalId];	
	s.y=0.0;
	float t = 0.f;		
	s.x = 10.0;
	p.w = 1.0f;
	if((float)(globalId)>time*1000.0){
		position[globalId] = (float4)(100000000.0,1000000000.0,10000000.0,1.0);	
	} else {
		position[globalId] = p;
	}
	astate[globalId] = s;
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