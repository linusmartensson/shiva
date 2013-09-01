
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
			__global float4* opos,				//10
			__read_only image2d_t worldimg,		
			__read_only image2d_t accelimg,
			__read_only image2d_t colorimg
			){
	
	const sampler_t st = CLK_NORMALIZED_COORDS_TRUE | CLK_FILTER_NEAREST | CLK_ADDRESS_REPEAT;		
	unsigned int globalId = get_global_id(0);
	
	float4 scp = screenposition[globalId];
	float4 p = position[globalId];
	float4 a = acceleration[globalId];
	float4 s = state[globalId];
	float4 c = color[globalId];
	float4 o = opos[globalId];
	
	float4 c0 = camera[0];
	float4 c1 = camera[1];
	float4 c2 = camera[2];
	float4 c3 = camera[3];
	
	if(s.y < minmaxsphere.x){
		//Pick random screen coordinate
		float r0 = randomvalue(1001, globalId, absoluteTime*0.1);
		float r1 = randomit(r0, 3521);
		
		float4 r=(float4)(r0,r1,r0,r1);
		p = read_imagef(worldimg, st, (float2)((r.x*2.0f-1.0f)*16.f/9.f, r.y*2.0f-1.0f));
		a = read_imagef(accelimg, st, (float2)((r.x*2.0f-1.0f)*16.f/9.f, r.y*2.0f-1.0f));
		c = read_imagef(colorimg, st, (float2)((r.x*2.0f-1.0f)*16.f/9.f, r.y*2.0f-1.0f));
		

		//Particle lifetime
		s.z = max(a.w,0.1f);
		s.y = fabs(minmaxsphere.y);
		
		p.w=1.0f;
		if(s.y < minmaxsphere.x){
			c = (float4)(0.0,0.0,0.0,0.0);
		} else { 
			o=p;
		}
	}
	
	s.y -= frameTime*fabs(s.z); 
	float4 op = p;
	p += (float4)(a.xyz,0.0)*0.001;
	
	//Control opacity based on lifetime.
	c.w = clamp(fabs(s.y-minmaxsphere.y)*fabs(s.y-minmaxsphere.x),0.0f,1.0f);
	

	//Generate screenspace coordinates.
	scp = (float4)(dot(p, c0),dot(p, c1),dot(p, c2),dot(p, c3));
		
	//Interpolate for motion blurring.
	//float4 oscp = didreset?scp:mix(scp, oldscreenposition[globalId], 0.65);

	//Give lines actual length instead of motion blurring.
	float4 oscp = (float4)(dot(op, c0),dot(op, c1),dot(op, c2),dot(op, c3));

	//Store particle states.
	oldscreenposition[globalId] = oscp;
	screenposition[globalId] = scp;	
	if(pause < 0.5){
		position[globalId] = p;
		acceleration[globalId] = a;
		state[globalId] = s;
		color[globalId] = c;
	}
	opos[globalId] = o;

}