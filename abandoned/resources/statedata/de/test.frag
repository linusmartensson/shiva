
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

float sdBox( vec4 p, vec3 b )
{
  vec3  di = abs(p.xyz) - b;
  float mc = max(di.x, max(di.y,di.z));
  return min(mc,length(max(di,0.0)));
}

float sdPlane( vec4 p, vec4 n )
{
	return dot(p.xyz,n.xyz) + n.w;
}


float dist(vec4 r){
	r.xz = mod(r.xz,100.0)-50.0;
	float d = 100000000.0;
//	d = distance(vec3(0.0,0.0,0.0),r.xyz)-20.0;
	d = min(d, sdBox(r, vec3(1.0,1.0,1.0)*20.0));
	return d;
}

void main(void)
{
	float d = -1.0;
	vec3 dir = vec3(normalize(vec3(uv.xy*2.0-1.0,1.0)*vec3(16.0/9.0,1.0,1.0)));
	dir = rot*dir;
	vec4 ray = vec4(0.0,0.0,0.0,1.0);
	ray += vec4(trans,0.0);
	float step = 0;
	float mstep = 500;
	float dm = 1.0;
	float lim = 0.1;

	if(skip<0.5){

		while(step < mstep){

			d = dist(ray);
			if(d < lim || d > 100000.0) {
				break;
			}	
			ray.xyz += dir * d * dm;
			step += 1.0;
		}
		w = vec4(ray.xyz,cos(uv.x*100.0+d*10000.0+step*110.73+uv.y*100.0)*0.1);
	}
	if(abs(d) < lim && d >= 0.0){
		a = vec4(0.0,0.0,0.0,10.0);
		vec4 cc = (ray*camera);
		c = vec4(35.0,5.0,5.0,1.0)*(1.4-(cc.z/(cc.w-1.0)));
		gl_FragDepth = cc.z/(cc.w-1.0);
	} else {
		w.w = 2.0;
		a = vec4(1.0,1.0,1.0,20.0);
		c = vec4(0.0,0.0,0.0,1.0);
		gl_FragDepth = 1.0f;
	}
}

