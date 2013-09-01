__kernel void engine(
			__global float4* data, 
			float pause,
			float time){
			
	unsigned int globalId = get_global_id(0);
	
	float4 p = data[globalId*2];
	float4 c = data[globalId*2+1];
	p.x+= (randomvalue(globalId, 112542, 0.0f)-0.5f)*time;
	p.y+= (randomvalue(globalId, 132542, 0.0f)-0.5f)*time;
	p.z+= (randomvalue(globalId, 122542, 0.0f)-0.5f)*time;
	data[globalId*2] = p;
	data[globalId*2+1] = c;
}
