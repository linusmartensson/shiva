

float iso(float4 pos, float time){
	pos*=1.0;
	float4 pz = pos; 
	float4 pz2 = pos; 
	float4 pz3 = pos;
	float sz = 520.0f;
	float sz2 = sz*0.5;
	pz = fmod(fabs(pz),(float4)(sz))-sz2;
	pz2.x -= sz2;
	pz2 = fmod(fabs(pz2),(float4)(sz))-sz2;
	pz3.z -= sz2;
	pz3 = fmod(fabs(pz3),(float4)(sz))-sz2;
	pz.y=pz2.y=pz3.y=pos.y;
	pz.w=pz2.w=pz3.w=0.0;

	float ssz = 200.0;

	float ww = 30.0+sin(pos.x*0.009+pos.y*0.0012+pos.z*0.0033+time*0.003)*20.0*tslbi(time)*0.01;
	float d =
		storus(
			pz,
			(float2)(
				ssz,
				ww
			)
		);
	d = dunion(
		storus(
			rotx(pz2,3.14159/2.0),
			(float2)(
				ssz,
				ww
			)
		),d);
	d = dunion(
		storus(
			rotz(pz3,3.14159/2.0),
			(float2)(
				ssz,
				ww
			)
		),d);


  d = dunion(d, splane(trans(pos, (float4)(0.0,40.0,0.0,0.0)), (float4)(0.0,1.0,0.0,0.0)));
  
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

		float4 a = oldAcceleration;
		float dist = clamp(iso(position,time),0.0,1.0);

		a = g(position,1.0,time)*(1.0-dist)+a*dist;

		//magnitude adjustment
		a.w = 400.0;

		return a;
}

float genLifespeed(int globalId, float time, float4 position, float4 screenposition){
	float r0 = randomvalue(1235,globalId, time);
	return randomvalue(r0, globalId, time)*10.0*tslbi(time)*0.01+5.0;
}

float4 genColors(int globalId, float time, float4 position, float4 prevColor, float4 screenpos){
	float4 c = (float4)(0.0);

			float4 normal = g(position, 1.0, time);
			//a.xyz = normal.xyz;
			
			float sz = 600;
			float sz2 = sz*0.5;
			float4 pz = position;
			pz.xz += sz2; 

			//There's a bug when pz goes negative. Make it far away.
			pz = fmod((pz+sz*1000.0),(float4)(sz))-sz2;
			pz.y = position.y;
			pz.w = 0.0;
			float4 light = (float4)(sin(time*0.001)*20.0,10.0,cos(time*0.001)*20.0,0.0)+pz;

			c += (float4)(0.5,0.4+screenpos.z*0.0003 ,0.1 +screenpos.z*0.0004,0.0);

			//Ambient occlusion.
			float m = 17.0;
			float  occ  = 1.0f;
				   occ -= (1.0*m-clamp(0.6*iso(position+normal*1.0*m,time),0.0,1.0*m))/2.0f/m;
				   occ -= (2.0*m-clamp(0.6*iso(position+normal*2.0*m,time),0.0,2.0*m))/4.0f/m;
				   occ -= (3.0*m-clamp(0.6*iso(position+normal*3.0*m,time),0.0,3.0*m))/8.0f/m;
				   occ -= (4.0*m-clamp(0.6*iso(position+normal*4.0*m,time),0.0,4.0*m))/16.0f/m;
			occ = pow(clamp(occ-0.05,0.0,1.0),1.3)*12.0;
			
			c *= clamp(occ,0.00,1.0);
			c.x -= screenpos.z*0.0001;
			
			//Ambient light
			c += (float4)(0.3 ,0.2,0.07,0);;
	return c*5.0;
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
	float stepsize = 0.7;

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
