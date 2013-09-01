

float iso(float4 pos, float time){
	
	pos*=1.0; 
	float4 pz = pos; 
	float sz = 30000.0f;
	float sz2 = sz*0.5;
	pz.x = pz.x;
	pz.y = pz.y+5001;
	pz.z = pz.z;
	 float f=max(0.0,(time+2000.0)*2.0-3000.0)*0.0001;
	f=f*f;
	f=min(f,100.f);
	
	float radi = 1000.0+cos(time*0.00014+pos.x*0.0009*f)*300.0-f*50.0;
	float d =
		storus(
			0.9*rotz(
				roty(
					rotx(
						pz
						,(time*0.00004+pos.y*0.00004*f+pos.z*0.00005*f)
						)
					,(time*0.000035+pos.z*0.00004*f+pos.x*0.00003*f)
					)
				,(time*0.000033+pos.x*0.00005*f+pos.y*0.00002*f)
				)
			,
			/*(float4)(0.0,1.0,0.0,0.0)
			300.0*/(float2)(
				3000.0+cos(time*0.00011+pos.x*0.0003*f)*300.0+f*400.0,
				radi
			)
		);

 	d = dunion(d, splane(trans(pos, (float4)(0.0,8000.0+0.0001*distance((float2)(0.0), pos.xy),0.0,0.0)), (float4)(0.0,1.0,0.0,0.0)));

	d = dunion(d, ssphere(pos, min(1500.0, max(-radi*2.0,0.0))));



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
		float f=max(0.0,(time+2000.0)*2.0-3000.0)*0.0001;
		f=f*f;
		f=min(f,100.f);
	


		float radi = 1000.0+cos(time*0.00014+position.x*0.0009*f)*300.0-f*50.0;

		float ma = -max(-1.0,min(radi+500.0,0.0));

		float4 a = oldAcceleration*(1.05-ma*0.02);

		a=mix(normalize(g(position,10.0,time))*2.0,a, clamp(iso(position,time),0.0,1.0));
		a.y+=0.07;
	
		//magnitude adjustment
		a.w = 1000.0+ma*50000.0*clamp(position.y+2000,0.0,1.0);

		return a;
}

float genLifespeed(int globalId, float time, float4 position, float4 screenposition){
	float r0 = randomvalue(1235,globalId, time);
	return randomvalue(r0, globalId, time)*5.0+2.0;
}

float4 genColors(int globalId, float time, float4 position, float4 prevColor, float4 screenpos){
	float4 c = (float4)(0.0);
	
	float4 pz = position; 
	float sz = 950.0f;
	float sz2 = sz*0.5;
	pz.x = fmod(fabs(pz.x),sz);
	pz.y = fmod(fabs(pz.y),sz);
	pz.z = fmod(fabs(pz.z),sz);
			
	float4 normal = g(position, 1.0, time);
	//a.xyz = normal.xyz;
	pz = rotz(
		roty(
			rotx(
				pz
				,time*0.0001+pz.y*0.009+pz.z*0.002
				)
			,time*0.00015+pz.z*0.004+pz.x*0.007
			)
		,time*0.00023+pz.x*0.005+pz.y*0.005
		);

//	c = (float4)(-5.0+sin(pz.z*0.03)*5.0,1.0+sin(pz.z*0.003)*1.0,1.0+sin(pz.y*0.003)*1.0,0);
	float f=max(0.0,(time+2000.0)*2.0-3000.0)*0.0001;
	f=f*f;
	f=min(f,100.f);
	
	float radi = 1000.0+cos(time*0.00014+position.x*0.0009*f)*300.0-f*50.0;
	

			
	c+=(position.y+8000)*0.001;
	c+=max(0.0,10.0-fabs(position.y+8000.0));
	float d=sqrt(dot(position,position))*0.001;
	return c*1.2*(float4)(0.3*d-(5.0-d)*max(-1.0,min(radi+500.0,0.0))*10.0,0.9,1.7,1.0);
}


__constant float inf = 20000.0;
__constant float md = 30000.0;
__constant int mstep = 8000;
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
	float stepsize = 0.6;

	while(s<mstep){
		d = iso(pos-trans, time);

		if(d<0.1f) {

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
