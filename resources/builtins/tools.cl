float randomvalue(int globalId, int seed, float time){
	globalId += 1;
	return 
			(((globalId*globalId*73313)%seed)
		    +globalId*(seed*31337*((int)(time*10000.0f+2.0f)))
			&16383)/16384.0f;
}

float randomit(float prev, int seed){
	prev+=1.0f;
	prev*=100000.0f;
	int iprev = (int)prev;
	return ((iprev*iprev+iprev*seed)&16383)/16384.0f;
}
float4 roty(float4 p, float ang){
	float4 t = p;

	p.x = t.z*sin(ang)+t.x*cos(ang);
	p.z = t.z*cos(ang)-t.x*sin(ang);

	return p;
}
float4 rotx(float4 p, float ang){
	float4 t = p;

	p.y = t.z*sin(ang)+t.y*cos(ang);
	p.z = t.z*cos(ang)-t.y*sin(ang);

	return p;
}
float4 rotz(float4 p, float ang){
	float4 t = p;

	p.x = t.y*sin(ang)+t.x*cos(ang);
	p.y = t.y*cos(ang)-t.x*sin(ang);

	return p;
}
float ssphere(float4 pos, float r){
	return distance(pos,(float4)(0.0,0.0,0.0,0.0)) - r;
}
float uroundbox(float4 p, float4 b, float r){
	return length(max(fabs(p)-b,0.0))-r;
}
float sbox(float4 p, float4 b){
	float4 di = fabs(p)-b;
	float mc = max(di.x,max(di.y, di.z));
	return min(mc, length(max(di,0.0)));
}
float storus(float4 p, float2 t){
	float2 q = (float2)(length(p.xz)-t.x,p.y);
	return length(q)-t.y;
}
float scone(float4 p, float2 c){
	c=normalize(c);
	float q = length(p.xy);
	return dot(c,(float2)(q,p.z));
}
float scylinder(float4 p, float4 c){
	return length(p.xz-c.xy)-c.z;
}
float splane(float4 p, float4 n){
	n = normalize(n);
	return dot(p,(float4)(n.xyz,0.0))+n.w;
}
float dunion(float d1, float d2){
	return min(d1,d2);
}
float dsubtract(float d1, float d2){
	return max(-d1,d2);
}
float dintersect(float d1, float d2){
	return max(d1,d2);
}
float4 rep(float4 p, float4 c){
	return fmod(p,c)-0.5*c;
}
float4 trans(float4 p, float4 s){
	return p+s;
}
float4 displace(float dist, float disp){
	return dist+disp;
}



float t(float c){
	const float beatdist = 1000.f*60/140.f;

	return (c-1.f)*beatdist*4.f;
}

float tslb(float time){
	const float beatdist = 1000.f*60/140.f;

	return fmod(time, beatdist);
}

float tslbi(float time){
	const float beatdist = 1000.f*60/140.f;

	return beatdist - tslb(time);
}

float toff(float time, float off){
	return max(time-t(off),0.0);
}

float timefac(float time, float off, float spd){
	return clamp(toff(time, off)*spd,0.0,1.0);	
}