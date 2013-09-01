

uniform sampler2D bt;
uniform sampler2D track;
uniform sampler2D pos;
uniform sampler2D wform;
uniform mat4 camera;
uniform float time;
uniform mat3 rot;
uniform vec3 trans;
in vec2 uv;
out vec4 texOut;

float sdCylinder( vec3 p, vec3 c )
{
  return length(p.xz-c.xy)-c.z;
}

float sdBox( vec3 p, vec3 b )
{
  vec3  di = abs(p.xyz) - b;
  float mc = max(di.x, max(di.y,di.z));
  return min(mc,length(max(di,0.0)));
}

float sdSphere( vec3 p, float s )
{
  return length(p.xyz)-s;
}

float sdPlane( vec3 p, vec4 n )
{
	return dot(p.xyz,n.xyz) + n.w;
}
float dista(vec3 r, float fa, float fk){
	vec3 um = r;
	float q = snoise(vec4(r.xyz*0.00004,0.1*time+30.0));
	

	float d = 100000000.0;
	d= (q+1.1)*10000.0;
	
	d=d+(length(r-vec3(0.0,0.0,40000.0))-30000);
	return d;
}
float diste(vec3 r, float fa, float fk){
	vec3 um = r;
	float q = snoise(vec3(r.yz*0.00002,0.1*time+30.0));
	float a = snoise(vec3(r.xz*0.00002,0.1*time+20.0));
	

	float d = 100000000.0;
	
	vec3 i =r;
	
	i.x+=q*10000.0;
	i.yz+=a*7000.0;
	r.xzy = mod(i.xzy,10000)-5000;
	
	d = min(d,sdBox(r, vec3(10000.0,1000.0,1000.0)));
	d = min(d,sdBox(r, vec3(1000.0,1000.0,10000.0)));
	i.z = mod(i.z,10000)-5000;
	d = max(d,sdBox(i, vec3(20000.0,5000.0,200000.0)));
	return d;
}
float distb(vec3 r, float fa, float fk, vec2 aa, vec2 ca, vec2 da, vec2 ga){
	float u = snoise(r.xz*0.00001)*5000.0;
	r.y += 25000;
	vec3 ja = r;
	vec3 jc = r;
	vec3 jd = r;
	vec3 jg = r;
	ja.xz = vec2(aa.x*r.x-aa.y*r.z,aa.y*r.x+aa.x*r.z);
	jc.xz = vec2(ca.x*r.x-ca.y*r.z,ca.y*r.x+ca.x*r.z);
	jd.xz = vec2(da.x*r.x-da.y*r.z,da.y*r.x+da.x*r.z);
	jg.xz = vec2(ga.x*r.x-ga.y*r.z,ga.y*r.x+ga.x*r.z);
	ja.xz = mod(ja.xz+27000.0,30000)-15000.0;
	jc.xz = mod(jc.xz+53000.0,50000)-25000.0;
	jd.xz = mod(jd.xz+37000.0,57000)-28500.0;
	jg.xz = mod(jg.xz+98000.0,77000)-38500.0;
	float z = min(min(sdBox(jc, vec3(10000,30000,10000)),
					  sdBox(ja, vec3(10000,20000,10000))),
				  max(sdBox(jd, vec3(20000,70000,20000)),
					  sdBox(jg, vec3(21000,72000,21000))));
	z = max(z,-sdBox(jd, vec3(11000,72000,11000)));
	z = max(z,-sdBox(mod(ja,1000)-500, vec3(300,400,400)));
	z = max(z,-sdBox(mod(jd,1000)-500, vec3(400,400,300)));
	
	//float u = 15000.0;
	z = min(z,sdPlane(r, vec4(0.0,1.0,0.0,u)));
	return z;
}
float dist(vec3 r, float fa, float fk, vec2 aa, vec2 ca, vec2 da, vec2 ga){
	return distb(r, fa, fk, aa,ca,da,ga);

}
vec3 norm(vec3 r, float fa, float fk, vec2 aa, vec2 ca, vec2 da, vec2 ga){
	vec2 e = vec2(1.0,0.0);
	float w = dist(r,fa,fk,aa,ca,da,ga);

	float x = dist(r+e.xyy, fa, fk,aa,ca,da,ga);
	float y = dist(r+e.yxy, fa, fk,aa,ca,da,ga);
	float z = dist(r+e.yyx, fa, fk,aa,ca,da,ga);

	return normalize(vec3(x,y,z)-w);
}

float ao(vec3 r, float fa, float fk, float dst, vec3 normal, vec2 aa, vec2 ca, vec2 da, vec2 ga){
	return max(dist(r+normal*dst, fa, fk,aa,ca,da,ga)/dst,0.0);
}
vec4 cmod(vec3 ray, float fa, float fk, vec2 aa, vec2 ca, vec2 da, vec2 ga, vec3 dir, float dm, float k, float tf, float dp, float step){
	vec3 or = ray.xyz+dir*step*dm;
	
	
	vec4 texOut = vec4(1.0,1.0,1.0,1.0);
	
	vec3 normal = norm(or, fa, fk,aa,ca,da,ga);
	float tt = time;
	vec3 lightdir = normalize(vec3(1.0,1.0,1.0));
	float lsc = snoise(vec4(or*vec3(1.0,0.1,0.1)*0.0002+time*0.1,time*0.01))*0.2;
	
	texOut.xyz = pow(max(dot(lightdir+lsc,normal.xyz),0.0),2.0)*vec3(0.3,0.5,1.0)*(lsc+1.0);
	float saoc =
		ao(or, fa, fk, 1000,normal,aa,ca,da,ga)*0.4+
		ao(or, fa, fk, 6000,normal,aa,ca,da,ga)*0.6; 
	float aoc = saoc*0.5+
		ao(or,fa,fk,20000,normal,aa,ca,da,ga)*0.5;
	
	texOut.xyz *= mix(
			vec3(1.0),
			abs(pow(snoise(or*0.005*(2.0-normal)),2.0)+1.5)*vec3(0.05,0.7,0.2)+
			0.0,
			clamp(vec3(
			abs(snoise(or*0.0001*(2.0-normal))+1.5)*0.4+
			snoise(or.xyz*0.001)*0.4+1.0-aoc*2.0),0.0,1.0));
	
	//Fade distance
	
	texOut.xyz += k*vec3(0.1,0.0,0.5)*0.01;
	texOut.xyz *= aoc*saoc;
	texOut.xyz *= mix(1.0,0.5+2.5*clamp(1.0-2.0*sqrt(distance(0.0, snoise(vec4(or*vec3(0.0001,0.00001,0.0001)+time*0.01,time*0.2)))),0.0,1.0)*0.3*clamp(dot(normal,vec3(0.0,1.0,0.0))*100.0,0.0,1.0),saoc);
	texOut = mix(vec4(tf*0.2+1.0)*vec4(0.02,0.00,0.20,1.0),texOut,1.0-smoothstep(1.0,dp,step));
	
	texOut.a=1.0;

	return texOut;
}




void main(void)
{
	float ar = 0.1;
	float dr = 0.7;
	float cr = 1.0;
	float gr = 1.9;
	vec2 aa = vec2(cos(ar), sin(ar));
	vec2 ca = vec2(cos(cr), sin(cr));
	vec2 da = vec2(cos(dr), sin(dr));
	vec2 ga = vec2(cos(gr), sin(gr));
	

	float fa = texture2D(pos,vec2(0.2,0.5)).r;
	float fk = texture2D(pos,vec2(0.1,0.5)).r;
	float d = -1.0;
	vec3 dir = vec3(normalize(vec3(uv.xy*2.0-1.0,1.0)*vec3(16.0/9.0,1.0,1.0)));
	
	dir = rot*dir;
	float tf = snoise(vec4(dir.xyz*vec3(2.1),time*0.1));
	dir += snoise(vec4(dir.xyz*2.0+trans*0.00001,time*0.1))*0.05;
	dir = normalize(dir);
	

	vec3 ray = vec3(0.0,0.0,0.0);
	ray += trans;
	float step = 1.0;
	float dm = 1.0;
	float lim = 0.1;
	float dp = 200000.0;
	float k=0.0;
	for(step=1.0;step < dp;){
		k+=1.0;
		d = dist(ray+dir*step*dm,fa,fk,aa,ca,da,ga);
		lim = 0.1+abs(100.0*step/dp)+k*0.01;
		if(d < lim) {
			break;
		}
		step += d;
	}
	step = min(step,dp);
	texOut = cmod(ray,fa,fk,aa,ca,da,ga,dir,dm,k,tf,dp,step);
}

//Shadow - buggy unfortunately :(
	/*
	float s = 1.0;
	step = 0.0;
	float sdp = d;
	ray.xyz = or;

	step=1.0;
	float md = 1.0;
	for(step=1.0;step<dp;){
		d = abs(dist(ray+lightdir*step*dm,fa,fk,aa,ca,da,ga));
		lim = 0.1;

		if(d < lim){
			break;
		}
		step += d;
	}*/