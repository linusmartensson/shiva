

__kernel void clzero(__global float4* value, float x, float y, float z, float w){
	value[get_global_id(0)] = (float4)(x,y,z,w);
}

float ra(float id, float k){
	return fmod(fmod(id*432615+325,413532)*214341+fmod(k*3215+101,41531)*433251, 23521.f)/23521.f;
}

__kernel void clboom(__global float4* p, float time, float k, float i){
	int f = get_global_id(0);
	p[f] = (float4)(ra(f+time,0+k), ra(f+time,1+k), ra(f+time,2+k), ra(f+time,3+k))*0.901f-i;

}

__kernel void doastate(__global float4* temp, __global float4* astate){
	
	unsigned int globalId = get_global_id(0);
	
	float b = astate[globalId].y;
	float c  = astate[globalId].x;
	
	int a = round(b);
	if(a == 0 || a == 1){
		c = -1;
	}
	else if((a == 2 || a == 3 )&& c > 0.0){
		c = 1;
	}
	else if((a == 4 ||a == 5 || a == 6 || a == 7 || a == 8 )&& c > 0.0){
		c = -1;
	}
	else if((a == 3) && c < 0.0){
		c = 1;
	}
	
	astate[globalId].x = c;
}
__kernel void doagrow(__global float4* temp, __global float4* astate){
	
	unsigned int globalId = get_global_id(0);
	
	float b = astate[globalId].y;
	float c  = astate[globalId].x;
	
	int a = round(b);
	if((a == 1 || a == 2 || a == 3|| a ==4 || a== 5 || a==6 || a==7 || a==8) && c <= 0.5 && c > -1){
		c = 1;
	}
	else if(c > 0.5){
		c = -1;
	}
	astate[globalId].x = c;
}
__kernel void clplane(__global float4* position, float time, __global float4 *astate, __global float4* temp){
			
	unsigned int globalId = get_global_id(0);
	
	int lim = sqrt(40000.f);

	float4 p = position[globalId];

	

	float4 px = (float4)(
	fmod(globalId,lim*1.f)-lim/2,
	-10.f,
	(globalId/lim)-lim*0.5,
	1.0f)*20.0f;
	p=px;;

	px.y += sin(p.z*0.01f+time+p.x*0.01f)*10.0f;
	
	
	p.w = 1.0f;
	position[globalId] = p;

}
__kernel void cldgrow(__global float4* position, float time, __global float4 *astate, __global float4* temp){
			
			
	unsigned int globalId = get_global_id(0);
	
	int lim = sqrt(40000.f);

	float4 p = position[globalId];

	

	float4 px = (float4)(
	fmod(globalId,lim*1.f)-lim/2,
	-10.f,
	(globalId/lim)-lim*0.5,
	1.0f)*20.0f;
	p=px;;
	
	float t = time*0.0;
	float aaa = 0.0+time*0.00003;
	float bbb = 0.0+time*0.00001;
	float ccc = 0.0+time*0.00002;
	p.x = cos(px.x*aaa+t)*px.x-sin(px.x*aaa+t)*px.z;
	p.z = sin(px.x*aaa+t)*px.x+cos(px.x*aaa+t)*px.z;
	//p.y += sin(p.z*0.01f+time+p.x*0.01f)*100.0f;
	float4 pu = p;
	p.y = cos(px.x*bbb)*pu.y - sin(px.x*bbb)*pu.z;
	p.z = sin(px.x*bbb)*pu.y + cos(px.x*bbb)*pu.z;
	pu = p;
	p.x = cos(px.x*ccc)*pu.x - sin(px.x*ccc)*pu.y;
	p.y = sin(px.x*ccc)*pu.x + cos(px.x*ccc)*pu.y;
	p.w = 1.0f;

	//p.y += sin(p.z*0.01f+time+p.x*0.01f)*100.0f;
	
	
	p.w = 1.0f;
	position[globalId] = p;
	
	float x;

	int q = globalId-1;
	if(q<0) q+=40000;
	x = astate[q].x>0.0?1.0:0.0;
	
	q = globalId+1;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId-lim;
	if(q<0) q+=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId+lim;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;
	
	q = globalId-lim-1;
	if(q<0) q+=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId+lim-1;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;
	
	q = globalId-lim+1;
	if(q<0) q+=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId+lim+1;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;
	
	astate[globalId].y = x;

}
__kernel void clconwaydist(__global float4* position, float time, __global float4 *astate, __global float4* temp){
			
	unsigned int globalId = get_global_id(0);
	
	int lim = sqrt(40000.f);

	float4 p = position[globalId];

	

	float4 px = (float4)(
	fmod(globalId,lim*1.f)-lim/2,
	-10.f,
	(globalId/lim)-lim*0.5,
	1.0f)*20.0f;
	p=px;;

	px.y += sin(p.z*0.01f+time+p.x*0.01f)*10.0f;
	
	float pa = px.x-lim*20.0;
	float ccc = 0.00001*time;
	p.z = cos(pa*ccc)*px.z - sin(pa*ccc)*px.y;
	p.y = sin(pa*ccc)*px.z + cos(pa*ccc)*px.y;
	
	p.w = 1.0f;
	position[globalId] = p;
	
	float x;

	int q = globalId-1;
	if(q<0) q+=40000;
	x = astate[q].x>0.0?1.0:0.0;
	
	q = globalId+1;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId-lim;
	if(q<0) q+=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId+lim;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;
	
	q = globalId-lim-1;
	if(q<0) q+=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId+lim-1;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;
	
	q = globalId-lim+1;
	if(q<0) q+=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId+lim+1;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;
	
	astate[globalId].y = x;


}
__kernel void clconway(__global float4* position, float time, __global float4 *astate, __global float4* temp){
			
	unsigned int globalId = get_global_id(0);
	
	int lim = sqrt(40000.f);

	float4 p = position[globalId];

	

	float4 px = (float4)(
		fmod(globalId,lim*1.f)-lim/2,
		-10.f,
		(globalId/lim)-lim*0.5,
	1.0f)*20.0f;
	p=px;;

	float t = max(time-61.0456f*0.1f,0.0f);
	

	px.y += sin(p.z*0.001f+t+p.x*0.001f)*100.0f*min(t,1.0f);

	t = -max(time-80.6561f*0.1f,0.0f);
	
	float pa = px.x-lim*20.0;
	float ccc = 0.0001*t;
	p.z = cos(pa*ccc)*px.z - sin(pa*ccc)*px.y;
	p.y = sin(pa*ccc)*px.z + cos(pa*ccc)*px.y;
	px=p;
	p.x = cos(pa*ccc)*px.x - sin(pa*ccc)*px.y;
	p.y = sin(pa*ccc)*px.x + cos(pa*ccc)*px.y;
	
	px=p;
	p.z = cos(pa*ccc)*px.z - sin(pa*ccc)*px.x;
	p.x = sin(pa*ccc)*px.z + cos(pa*ccc)*px.x;
	
	
	
	p.w = 1.0f;
	position[globalId] = p;
	
	float x;

	int q = globalId-1;
	if(q<0) q+=40000;
	x = astate[q].x>0.0?1.0:0.0;
	
	q = globalId+1;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId-lim;
	if(q<0) q+=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId+lim;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;
	
	q = globalId-lim-1;
	if(q<0) q+=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId+lim-1;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;
	
	q = globalId-lim+1;
	if(q<0) q+=40000;
	x += astate[q].x>0.0?1.0:0.0;

	q = globalId+lim+1;
	if(q>=40000) q-=40000;
	x += astate[q].x>0.0?1.0:0.0;
	
	astate[globalId].y = x;


}
__kernel void clline(__global float4* position, float time){
			
	unsigned int globalId = get_global_id(0);
	
	int lim = sqrt(40000.f);

	float4 p = position[globalId];


	float4 px = (float4)(
	(globalId/2)*0.1f,
	-10.f,
	fmod((globalId)*1.f,2.0f)*250,
	1.0f)*(max(time-40.f,1.0f)*0.01+0.99);
	p = px;
	float t = time*0.01;
	float aaa = 0.02+time*0.0011;
	float bbb = 0.01+time*0.0013;
	float ccc = 0.03+time*0.0017;
	p.x = cos(px.x*aaa+t)*px.x-sin(px.x*aaa+t)*px.z;
	p.z = sin(px.x*aaa+t)*px.x+cos(px.x*aaa+t)*px.z;
	float4 pu = p;
	p.y = cos(px.x*bbb)*pu.y - sin(px.x*bbb)*pu.z;
	p.z = sin(px.x*bbb)*pu.y + cos(px.x*bbb)*pu.z;
	pu = p;
	p.x = cos(px.x*ccc)*pu.x - sin(px.x*ccc)*pu.y;
	p.y = sin(px.x*ccc)*pu.x + cos(px.x*ccc)*pu.y;
	p.w = 1.0f;
	position[globalId] = p;
}
__kernel void clline2(__global float4* position, float time){
			
	unsigned int globalId = get_global_id(0);
	
	int lim = sqrt(40000.f);

	float4 p = position[globalId];


	float4 px = (float4)(
	(globalId/2)*0.1f,
	-10.f,
	fmod((globalId)*1.f,2.0f)*250,
	1.0f)*(max(time-40.f,1.0f)*0.01+0.99);
	p = px;
	float t = time*0.01;
	float aaa = 0.02+time*0.0041;
	float bbb = 0.01+time*0.0043;
	float ccc = 0.03+time*0.0047;
	p.x = cos(px.x*aaa+t)*px.x-sin(px.x*aaa+t)*px.z;
	p.z = sin(px.x*aaa+t)*px.x+cos(px.x*aaa+t)*px.z;
	float4 pu = p;
	p.y = cos(px.x*bbb)*pu.y - sin(px.x*bbb)*pu.z;
	p.z = sin(px.x*bbb)*pu.y + cos(px.x*bbb)*pu.z;
	pu = p;
	p.x = cos(px.x*ccc)*pu.x - sin(px.x*ccc)*pu.y;
	p.y = sin(px.x*ccc)*pu.x + cos(px.x*ccc)*pu.y;
	p.w = 1.0f;
	position[globalId] = p;
}
