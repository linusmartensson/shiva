

float iso(float4 pos, float time){
	
 	float d = splane(trans(pos,(float4)(0.0,1000.0-sin(pos.x*0.001+time*0.001)*200.0-distance(pos,0.0)*0.2,0.0,0.0)), (float4)(0.0,1.0,0.0,1.0));

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
		a.y+=0.08/(time*0.01+1.0);
		a.x+=sin(position.y*0.006)*0.05;
		a.z+=cos(position.y*0.006)*0.05;

		//magnitude adjustment
		a.w = 1000.0;

		return a;
}

float genLifespeed(int globalId, float time, float4 position, float4 screenposition){
	float r0 = randomvalue(1235,globalId, time);
	return randomvalue(r0, globalId, time)*1.0+0.9;
}

float4 genColors(int globalId, float time, float4 position, float4 prevColor, float4 screenpos){
	
	float4 c = (float4)(1.0,0.9,0.4,1.0);
			
	float d = iso(position, time);
	c.z += d*0.001;
	c.y *= 0.3;
	c.x -= d*0.003;

	return c*2.0;
}


__constant float inf = 20000.0;
__constant float md = 30000.0;
__constant int mstep = 8000.0;
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
	float stepsize = 0.8;

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
