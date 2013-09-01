

float iso(float4 pos, float time){
	
	pos*=2.0;
	float4 pz = pos;
	

	pos = rotx(pos,3.14159/4.0+time*0.0001);
	pos = rotz(pos,3.14159/4.0+time*0.0001);
	



	float d =
		ssphere(
			pos,
			1000.0
		);
	
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

		//magnitude adjustment
		a.w = 100000.0;

		return a;
}

float genLifespeed(int globalId, float time, float4 position, float4 screenposition){
	float r0 = randomvalue(1235,globalId, time);
	return randomvalue(r0, globalId, time)*1.0+1.0;
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
	float d = distance(position, (float4)(0.0));
	c = d*(float4)(1.0,0.6,0.4,0.5)*0.0001;
	c += (5000.0-d)*0.00009;
			
	return c*20.0*time*0.001;
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
	float stepsize = 0.3;

	while(s<mstep){
		d = iso(pos-trans, time);

		if(d<0.3f) {

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
