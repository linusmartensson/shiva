
__constant float2 minmaxsphere = (float2)(0.5f, 1.5f);

__kernel void particleengine(
			__global float4* position,			//0
			__global float4* acceleration, 
			__global float4* state, 
			__global float4* color,
			float frameTime,
			float absoluteTime,					//5
			__constant float4* camera,
			__global float4* screenposition,
			__global float4* oldscreenposition,
			float pause,
			__read_only image2d_t worldimg,		//10
			__read_only image2d_t accelimg,
			__read_only image2d_t colorimg){
	
	const sampler_t st = CLK_NORMALIZED_COORDS_TRUE | CLK_FILTER_NEAREST | CLK_ADDRESS_REPEAT;		
	unsigned int globalId = get_global_id(0);
	
	float4 scp = screenposition[globalId];
	float4 p = position[globalId];
	float4 a = acceleration[globalId];
	float4 s = state[globalId];
	float4 c = color[globalId];
	
	bool didreset = false;
	if(s.y < minmaxsphere.x){
		didreset = true;	
		//Pick random screen coordinate
		float r0 = randomvalue(1001, globalId, absoluteTime);
		float r1 = randomit(r0, 3521+absoluteTime);

		//Read deferred data
		p = read_imagef(worldimg, st, (float2)((r0*2.0f-1.0f)*16.f/9.f, r1*2.0f-1.0f));
		a = read_imagef(accelimg, st, (float2)((r0*2.0f-1.0f)*16.f/9.f, r1*2.0f-1.0f));
		c = read_imagef(colorimg, st, (float2)((r0*2.0f-1.0f)*16.f/9.f, r1*2.0f-1.0f));
		
		//Particle lifetime
		s.z = max(a.w,1.0f);
		s.y = minmaxsphere.y-fabs(p.w);
		if(s.y < minmaxsphere.x){
			c = (float4)(0.0,0.0,0.0,0.0);
		}
		p.w=1.0f;
	}
	
	s.y -= frameTime*fabs(s.z)*(1.0-pause)*0.5; 
	p += (float4)(a.xyz,0.0)*a.w*(1.0-pause)*0.001;
	
	//Control opacity based on lifetime.
	c.w = clamp(fabs(s.y-minmaxsphere.y)*fabs(s.y-minmaxsphere.x),0.0f,1.0f);
	
	float4 c0 = camera[0];
	float4 c1 = camera[1];
	float4 c2 = camera[2];
	float4 c3 = camera[3];

	//Generate screenspace coordinates.
	scp = (float4)(dot(p, c0),dot(p, c1),dot(p, c2),dot(p, c3));
		
	//Interpolate for motion blurring.
	float4 oscp = didreset?scp:mix(scp, oldscreenposition[globalId], 0.75);

	//Store particle states.
	oldscreenposition[globalId] = oscp;
	screenposition[globalId] = scp;	
	position[globalId] = p;
	acceleration[globalId] = a;
	state[globalId] = s;
	color[globalId] = c;
}