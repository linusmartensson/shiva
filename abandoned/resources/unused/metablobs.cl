float iso(float4 pos, float time){
	time *= 0.1;
	float4 t0 = (float4)(
		cos(time*0.001+sin(time*0.0017))*150.0,
		cos(time*0.001+sin(time*0.0021))*150.0-200.0,
		cos(time*0.001+sin(time*0.0014))*150.0
		,0.0);
	float4 t1 = (float4)(
		cos(time*0.0012+sin(time*0.0025))*150.0,
		cos(time*0.0011+sin(time*0.0014))*150.0-200.0,
		cos(time*0.0012+sin(time*0.0015))*150.0,
		0.0);
	float4 t2 = (float4)(
		cos(time*0.0015+sin(time*0.0035))*150.0,
		cos(time*0.0013+sin(time*0.0024))*150.0-200.0,
		cos(time*0.0011+sin(time*0.0025))*150.0,
		0.0);
	float4 t3 = (float4)(
		cos(time*0.0018+sin(time*0.0022))*150.0,
		cos(time*0.0017+sin(time*0.0029))*150.0-200.0,
		cos(time*0.0015+sin(time*0.0031))*150.0,
		0.0);
	float4 t4 = (float4)(
		cos(time*0.001+sin(time*0.0015))*150.0,
		cos(time*0.0016+sin(time*0.0019))*150.0-200.0,
		cos(time*0.0014+sin(time*0.001))*150.0,
		0.0);
	float4 t5 = (float4)(
		cos(time*0.0021+sin(time*0.0003))*150.0,
		cos(time*0.0015+sin(time*0.0027))*150.0-200.0,
		cos(time*0.0011+sin(time*0.0031))*150.0,
		0.0);

	pos = fmod(fabs(pos),900)-450;
	pos.w=0.0;

	float sph0 = ssphere(trans(pos,t0), 50.0f);
	float sph1 = ssphere(trans(pos,t1), 50.0f);
	float sph2 = ssphere(trans(pos,t2), 50.0f);
	float sph3 = ssphere(trans(pos,t3), 50.0f);
	float sph4 = ssphere(trans(pos,t4), 50.0f);
	float sph5 = ssphere(trans(pos,t5), 50.0f);	
	return 1.0/(
			1.0/(sph0+0.01)+
			1.0/(sph1+0.01)+
			1.0/(sph2+0.01)+
			1.0/(sph3+0.01)+
			1.0/(sph4+0.01)+
			1.0/(sph5+0.01)+
	//		1.0/(splane(trans(pos,(float4)(0.0,100.0,0.0,0.0 )),(float4)(0.0,1.0,0.0,1.0))+0.01)+
			0.0)-10.0;
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

		float4 a = oldAcceleration*0.95;

		//direction
		float r0 = randomvalue(5431,globalId, 1.0);
		float r1 = randomit(r0, 5431);
		float r2 = randomit(r1, 9123);
		float r3 = randomit(r2, 3123);
		a.x+=r0*0.01;
		a.y+=r1*0.01;
		a.z+=r2*0.01;
		a.xyz -= 0.5*0.01;

		//a += -0.05*(float4)(0.0,-position.y,0.0,0.0)*clamp(sbox(position, (float4)(100.0,2000.0,100.0,0.0)),-0.01,0.0);
		
		//magnitude adjustment
		a.w += 20.0+25.0*r3;

		return a;
}

float genLifespeed(int globalId, float time, float4 position, float4 screenposition){
	float r0 = randomvalue(1235,globalId, time);
	return randomvalue(r0, globalId, time)*10.0+1.0;
}

float4 genColors(int globalId, float time, float4 position, float4 prevColor, float4 screenpos){
	float4 c = (float4)(0.0);

			float4 normal = g(position, 1.0, time);
			c += (float4)(1.8-screenpos.z*0.0005,0.1+screenpos.z*0.004,0.1+screenpos.z*0.007,0.0);
			c = clamp(c, (float4)(0.0), (float4)(1.0));
			
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
	float stepsize = 0.5;

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
