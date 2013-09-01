float iso(float4 pos, float time){
	pos*=1.0;
	float4 pz = pos; 
	float sz = 150.0f;
	float sz2 = sz*0.5;
	pz.x = fmod(fabs(pz.x),sz)-sz2;
	//pz.y = fmod(fabs(pz.y),sz)-sz2;
	pz.z = fmod(fabs(pz.z),sz)-sz2;
	
	float4 pz2 = pos;
	sz = 600.0f;
	sz2 = sz*0.5;
	pz2-=sz2;
	pz2 = fmod(fabs(pz2),(float4)(sz))-sz2;
	pz2.y = pos.y;
	pz2.w = 0.0;
//	float w = 10.0*(cos(pos.y*0.02+time*0.001)+4.5);
	float w = 20.0;
	float pillars = sbox(pz, (float4)(w,150.0,w,0.0));
	float roof = splane(trans(pos,(float4)(0,-75,0,0)), (float4)(0,-1,0,0));
	float floor = splane(trans(pos,(float4)(0,125,0,0)), (float4)(0,1,0,0));
	float wall0 = splane(trans(pz2,(float4)(125,0,0,0)), (float4)(1,0,0,0));
	float wall1 = splane(trans(pz2,(float4)(-125,0,0,0)), (float4)(-1,0,0,0));
	float wall2 = splane(trans(pz2,(float4)(0,0,125,0)), (float4)(0,0,125,0));
	float wall3 = splane(trans(pz2,(float4)(0,0,-125,0)), (float4)(0,0,-125,0));

	float hallway = dunion(dunion(roof, floor),dintersect(dunion(wall0,wall1),dunion(wall2,wall3)));

	return dunion(pillars, hallway);
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
		a.y+=r1*0.01+0.1;
		a.z+=r2*0.01;
		a.xyz -= 0.5*0.01;

		//a += -0.05*(float4)(0.0,-position.y,0.0,0.0)*clamp(sbox(position, (float4)(100.0,2000.0,100.0,0.0)),-0.01,0.0);
		
		//magnitude adjustment
		a.w += 20.0+25.0*r3;

		return a;
}

float genLifespeed(int globalId, float time, float4 position, float4 screenposition){
	float r0 = randomvalue(1235,globalId, time);
	return randomvalue(r0, globalId, time)*1.0+1.0;
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

 			float lightangle = dot(-normal*(float4)(1.0,1.0,1.0,0.0),normalize(light));
			c += (float4)(0.8-screenpos.z*0.0005,0.1+screenpos.z*0.004,0.1+screenpos.z*0.007,0.0);
			c = clamp(c, (float4)(0.0), (float4)(1.0));
			c *= max(0.0,10.5-length(light)*0.025);
			
			//Ignore light angle if we're not on a surface.
			float dist = clamp(iso(position, time)*.2,0.0,1.0);
			
			//Adjust lighting by angle to light.
			c *= lightangle*(1.0-dist)+dist;

			//Ambient occlusion.
			float m = 7.0;
			float  occ  = 1.0f;
				   occ -= (1.0*m-clamp(0.6*iso(position+normal*1.0*m,time),0.0,1.0*m))/2.0f/m;
				   occ -= (2.0*m-clamp(0.6*iso(position+normal*2.0*m,time),0.0,2.0*m))/4.0f/m;
				   occ -= (3.0*m-clamp(0.6*iso(position+normal*3.0*m,time),0.0,3.0*m))/8.0f/m;
				   occ -= (4.0*m-clamp(0.6*iso(position+normal*4.0*m,time),0.0,4.0*m))/16.0f/m;
			occ = pow(clamp(occ-0.05,0.0,1.0),1.3)*12.0;
			
			c *= clamp(occ,0.00,1.0);
			
			//Ambient light
			c += (float4)(0.07,0,0.07,0);;
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
