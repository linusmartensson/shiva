

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

float sdSphere( vec3 p, float f )
{
  return length(p)-f;
}

float sdBox( vec3 p, vec3 b )
{
  vec3  di = abs(p.xyz) - b;
  float mc = max(di.x, max(di.y,di.z));
  return min(mc,length(max(di,0.0)));
}

float sdPlane( vec3 p, vec4 n )
{
	return dot(p.xyz,n.xyz) + n.w;
}

float sdCross(vec3 r, float s){
	return min(length(r.xz), min(length(r.xy), length(r.yz)))-s;
}

float modCross(vec3 r, float s){
	return sdCross(mod(r+s, vec3(s*2.0))-s,s/3.0);
}

float dista(vec3 r, float fa, float fk){
	vec3 um = r;
	float z = 1000000000;
	float p = time*0.01;
	vec3 cs = vec3(cos(p), sin(p), 0.0);
	cs.z = -cs.y;
	
	z = (length(um.xy)-15000.0+sin(um.z*0.00001)*10000.0);
	for(float i=1.0;i<32.0;i*=2.0){
		r.xy = cs.xy*r.x + cs.zx*r.y;	
		z = min(modCross(vec3(r.y, cs.xy*r.z+cs.zx*r.x).yxz,10000.0/i),z);
	}

	return max(length(um.xy) - 100000.0,-z);
}
float dist(vec3 r, float fa, float fk, vec2 aa, vec2 ca, vec2 da, vec2 ga){
	return dista(r, fa, fk);

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
	float lsc = 0.0;
	
	texOut.xyz = vec3(0.0);
	
	//Fade distance
	
	texOut.xyz += k*vec3(0.1,0.0,0.5)*0.01;
	
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
	//dir += snoise(vec4(dir.xyz*2.0+trans*0.00001,time*0.1))*0.05;
	dir = normalize(dir);
	

	vec3 ray = vec3(0.0,0.0,0.0);
	ray += trans;
	float step = 1.0;
	float dm = 1.0;
	float lim = 0.1;
	float dp = 2000000.0;
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