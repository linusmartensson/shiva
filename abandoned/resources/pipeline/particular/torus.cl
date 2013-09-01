



float iso(float4 pos, float time){
	
	float t1 =toff(time,9.0);
	float distort1 = timefac(time,9.0,0.001);
	float t2 =max(time-t(13.0f),0.0);
	float distort2 = clamp(t2*0.001,0.0,1.0);
	float distort3 = timefac(time,14.0,0.001);
	float distort4 = timefac(time,15.0,0.001);
	pos*=1.0;
	float4 pz = pos; 
	float sz = 950.0f;
	float sz2 = sz*0.5;
	//pz.x = fmod(fabs(pz.x+pz.z*0.001),sz)-sz2;
	//pz.y = fmod(fabs(pz.y),sz)-sz2;
	//pz.z = fmod(fabs(pz.z),sz)-sz2;

	float d =
		storus(
			rotz(
				roty(
					rotx(
						pz
						,t1*0.0004+pos.x*0.01*distort2
						)
					,t1*0.00035+pos.y*0.008*distort3
					)
				,(t1*0.00033)+pos.z*0.006*distort4
				)
			,
			/*(float4)(0.0,1.0,0.0,0.0)
			300.0*/(float2)(
				300.0+time*0.001+sin(t1*0.0011+pos.x*0.003)*30.0*distort1,
				50.0+sin(t1*0.0014+pos.x*0.009)*30.0*distort1
			)
		);

 //	d = dunion(d, splane(trans(pos, (float4)(0.0,400.0,0.0,0.0)), (float4)(0.0,1.0,0.0,0.0)));

	return d;
}




float4 g(float4 p, float ep, float time) {
	float4 e = (float4)(ep, 0.0, 0.0,0.0);
	p.w = 0.0;
	return normalize(
		
			(float4)(
				iso(p+e.xyyy, time), 
				iso(p+e.yxyy, time), 
				iso(p+e.yyxy, time),
				0.0
				)
	);
}

float4 genAcceleration(int globalId, float time, float4 position, float4 oldAcceleration, float4 screenpos){

		float4 a = oldAcceleration*1.0;

		a=mix(normalize(g(position,1.0,time))*1.0,oldAcceleration, clamp(iso(position,time),0.0,1.0));


		float beatreact = timefac(time, 17, 0.001)*(1.0-timefac(time, 32, 0.001));

		//magnitude adjustment
		a.w *= 0.1;
		a.w = 20+20.0*tslb(time)*beatreact;

		return a;
}

float genLifespeed(int globalId, float time, float4 position, float4 screenposition){
	float r0 = randomvalue(1235,globalId, time);
	return randomvalue(r0, globalId, time)*10.0*(1.0-timefac(time, 32, 0.001))+1.0;
}

float4 genColors(int globalId, float time, float4 position, float4 prevColor, float4 screenpos){
float4 c = (float4)(0.0);
			
			float4 normal = g(position, 1.0, time);
			//a.xyz = normal.xyz;
			float4 pz = rotz(
				roty(
					rotx(
						position
						,time*0.0001+position.y*0.009+position.z*0.002
						)
					,time*0.00015+position.z*0.004+position.x*0.007
					)
				,time*0.00023+position.x*0.005+position.y*0.005
				);

			c = (float4)(1.0+sin(pz.z*0.003)*2.0,2.0+sin(pz.z*0.003)*2.0,4.0+sin(pz.y*0.003)*4.0,0);
			
	return c;
}


__constant float inf = 2000.0;
__constant float md = 3000.0;
__constant int mstep = 80;
float4 run(float4 p, unsigned int globalId, unsigned int absoluteTime, __constant float4* roti, __constant float4* transi, float time){

	float4 trans = transi[0];
	
	p.x *= 16.0f/9.f;
	
    float4 opn = p;
	p.x = dot(opn, roti[0]);
	p.y = dot(opn, roti[1]);
	p.z = dot(opn, roti[2]);
	p = normalize(p);
	
	p.w = 0.0;
	int s = 0;
	float d = 0.0;
	float4 pos = 0.0;
	float stepsize = 0.2;

	while(s<mstep){
		d = iso(pos-trans, time);

		if(d<0.01f) {

			return pos;
		} else if (d>inf ||length(pos)>md){
			pos.w = mstep+100.0;
			return pos;
		} else {
			pos += p * d*stepsize;
		}
		s++;
	}
	return pos;
}
