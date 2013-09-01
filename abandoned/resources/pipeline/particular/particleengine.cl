
__constant float2 minmaxsphere = (float2)(0.5f, 1.5f);

__kernel void particleengine(
			__global float4* position, 
			__global float4* acceleration, 
			__global float4* state, 
			__global float4* color,
			float frameTime,
			float absoluteTime,
			__constant float4* camera,
			__global float4* screenposition,
			__global float4* oldscreenposition,
			float pause,
			__constant float4* rot,
			__constant float4* trans){
			
	unsigned int globalId = get_global_id(0);
	
	float4 scp = screenposition[globalId];
	float4 p = position[globalId];
	float4 a = acceleration[globalId];
	float4 s = state[globalId];
	float4 c = color[globalId];
	
	
	s.y -= frameTime*(s.z)*(1.0-pause)*0.5; 
	p += (float4)(a.xyz,0.0)*a.w*(1.0-pause)*0.001;
	
	float4 c0 = camera[0];
	float4 c1 = camera[1];
	float4 c2 = camera[2];
	float4 c3 = camera[3];
	bool didreset = false;
	if(s.y < minmaxsphere.x){
		didreset = true;
		float r0 = randomvalue(1001, globalId, absoluteTime);
		float r1 = randomit(r0, 3521+absoluteTime);
		float r2 = randomit(r1, 7221+absoluteTime);	
		
		p = run( (float4)((r0*2.0f-1.0f)*16.f/9.f, r1*2.0f-1.0f, 1.0, 0.0), 
				 globalId, 
				 absoluteTime, 
				 rot, 
				 trans, 
				 absoluteTime);
		if(p.w+0.5 > mstep){ //If we break out on an error condition, the p.w variable is set to an error flag which hides the particle.
			s.y = 0.0;
		} else {
			p.w=1.0;
			p = p-trans[0];
			
			//Compute screenspace particle and reset motion blur data.
			scp = (float4)(dot(p, c0), dot(p, c1), dot(p, c2), dot(p, c3));	

			//Particle lifetime
			s.y = minmaxsphere.y;
			s.z = genLifespeed(globalId, absoluteTime, p, scp);			
		}
		//If particle is far away from an edge on creation, it's probably @ infinity, only it hit mstep.
		s.w = iso(p,absoluteTime)<20.0?1.0:0.0;
	}
	
	//Acceleration
	a = genAcceleration(globalId, absoluteTime, p, didreset?(float4)(0.0):a, scp);

	//Coloring
	c = genColors(globalId, absoluteTime, p, didreset?(float4)(0.0):c, scp);

	//Hide particles on probable error conditions.
	c = s.y>minmaxsphere.x?c*s.w:0.0;

	//Control opacity based on lifetime.
	c.w = clamp(fabs(s.y-minmaxsphere.y)*fabs(s.y-minmaxsphere.x),0.0f,1.0f);
	
	//Generate screenspace coordinates.
	scp = (float4)(dot(p, c0),dot(p, c1),dot(p, c2),dot(p, c3));

	//Interpolate for motion blurring.
	float4 oscp = didreset?scp:mix(scp, oldscreenposition[globalId], 0.95);

	//Store particle states.
	oldscreenposition[globalId] = oscp;
	screenposition[globalId] = scp;	
	position[globalId] = p;
	acceleration[globalId] = a;
	state[globalId] = s;
	color[globalId] = c;
}
