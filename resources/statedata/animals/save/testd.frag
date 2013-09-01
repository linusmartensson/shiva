
#version 330
layout(location=0) out vec4 w;
layout(location=1) out vec4 a;
layout(location=2) out vec4 c;
in vec2 uv;
uniform mat4 camera;
uniform float time;
uniform mat3 rot;
uniform vec3 trans;
uniform float skip;

float sdBox( vec3 p, vec3 b )
{
  vec3  di = abs(p) - b;
  float mc = max(di.x, max(di.y,di.z));
  return min(mc,length(max(di,0.0)));
}

float sdPlane( vec3 p, vec4 n )
{
	return dot(p,n.xyz) + n.w;
}


float dist(vec3 r){
	r.xz=r.zx;
	float d = 100000000.0;

//	d = distance(vec3(0.0,0.0,0.0),vec3(mod(r.xz,2000.0)-1000.0,r.y).xzy)-200.0;

	vec3 a = r;
	a.x = mod(a.x,1000)-500;
	d = min(d, sdBox(a+vec3(0.0,300.0,0.0), vec3(10000.0,50.0,300.0)));
	d = min(d, sdBox(a+vec3(0.0,300.0,-400.0), vec3(100.0,3000.0,100.0)));
	d = min(d, sdBox(a+vec3(0.0,300.0,400.0), vec3(100.0,3000.0,100.0)));

	return d;
}
vec3 norm(vec3 r){
	vec2 e = vec2(1.0,0.0);
	float w = dist(r);

	float x = dist(r+e.xyy);
	float y = dist(r+e.yxy);
	float z = dist(r+e.yyx);

	return normalize(vec3(x,y,z)-w);
}
float ao(vec3 r, float dst, vec3 normal){
	return max(dist(r+normal*dst)/dst,0.0);
}
void main(void)
{
	float d = -1.0;
	vec3 dir = vec3(normalize(vec3(uv.xy*2.0-1.0,1.0)*vec3(16.0/9.0,1.0,1.0)));
	dir = rot*dir;
	vec3 ray = vec3(0.0,0.0,0.0);
	ray = trans;
	float step = 0;
	float mstep = 5000;
	float dm = 0.95;
	float lim = 0.1;
	float ds = 1.0;
	
	while(step < mstep){

		d = dist(ray + dir * ds);
		if(abs(d) < lim || d > 10000.0) {
			break;
		}
		ds += d*dm;
		step += 1.0;
	}
	ray = trans+dir*ds;
	w = vec4(ray,1.0);
	vec3 n = norm(ray);
	if(abs(d) < lim && d >= 0.0){
		a.xyz = -n*100.0;
		a.w=2.0+sin(dot(ray,ray));
		a.w*=1.0;
		vec4 cc = (w*camera);
		c.rgb = vec3(10.0)*dot(norm(ray),vec3(0.3,0.7,0.5))*ao(ray, 1.0, n)*ao(ray, 10.0, n)*ao(ray, 30.0, n);
		c.a = 1.0;
		
		gl_FragDepth = cc.z/(cc.w-1.0);
	} else {
		a = vec4(1.0,1.0,1.0,200.0);
		c = vec4(0.0,0.0,0.0,1.0);
		gl_FragDepth = 1.0f;
	}
}

