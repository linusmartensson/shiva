#line 2 0

uniform float t;
uniform vec3 camerapos;
uniform mat3 camerarot;
uniform vec2 resolution;
uniform vec2 texresolution;
uniform float depthout;
uniform sampler2D depthin;
uniform float usedepthin;
in vec2 uv;

layout(location = 0) out vec4 c;

struct isodata{
	float d;
	vec3 color;
	float normmult;
};

isodata miniso(isodata l, isodata r){
	isodata n;
	float d = step(l.d,r.d);
	
	n.d = mix(r.d,l.d,d);
	n.color = mix(r.color,l.color,d);
	n.normmult = mix(r.normmult,l.normmult,d);
	return n;
}

float box(vec3 p, vec3 sz){
	float di = 0.0f;
	vec3 d = abs(p) - sz;
	vec3 dd = d*d;
	di += d.x>0.0?dd.x:0.0;
	di += d.y>0.0?dd.y:0.0;
	di += d.z>0.0?dd.z:0.0;
	
	
	return di;
}
/*float cube(vec3 v, float sz){
	v = v*sign(v)-sz;
	return length(max(v,vec3(0.0)));
}*/
float cube(vec3 v, float sz){
	v = abs(v) - sz;
	return min(max(v.x,max(v.y,v.z)), length(max(v,0.0)));
}

float sphere(vec3 p, float r){
	return distance(p,vec3(0.0)) - r;
}
isodata iso(vec3 i){
	vec3 p = i;

	float rep = 3.5;
 	p = vec3(mod(p,rep)) - rep*0.5;
	p.y=i.y;
	
	isodata floor;

	floor.d = -sphere(i+camerapos*0.001,100.0);
	floor.color = vec3(cos(p.x)*10.0,cos(p.z )*10.0,cos(p.y)*10.0);
	floor.normmult = -1.0;
	isodata d;
//	p = p+vec3(0.0,sin(i.x*0.3+t)*2.0+cos(i.z*0.24+t*0.3)*2.0,0.0);
// 	d.d = max(cube(p, 1.0),-sphere(p,2.01))-0.6;
	d.d = cube(p,rep*.2);
//	d.d = p.y+1;
//	d.d = max(cube(p,rep*0.2), -sphere(p,rep*0.24));//snoise(vec4(i*1.0,t*0.1)));
//	d.d = d.d
		+0.3*( snoise(vec2(i.xz*0.4+722+vec2(t*1.1,0.0)*0.0)))
		+7.0*( snoise(vec2(i.xz*0.03+522+vec2(t*0.03,0.0)*0.0)))
		+1.0*( snoise(vec2(i.xz*0.2+222+vec2(t*0.2,0.0)*0.0)))
;
	d.color = vec3(30)*d.d;
	d.normmult = 1.0;

	isodata water;

	water.d = 1000.0;//i.y+4.0+sin(t+i.x*3.9+i.z*0.9+cos(i.z*4.9+t*0.7)*sin(i.x*7.3+t*0.1))*0.1;
	water.color = vec3(0.0,0.0,0.0);

	isodata sph;
	sph.d = sphere(i+vec3(0.0,-10.0,5.0), 1.5);
	sph.color = d.color;
	sph.normmult = 1.0;
	return 
		miniso(sph, miniso(miniso(d,water),floor));
}

vec3 g(vec3 p, float ep) {
	vec2 e = vec2(ep, 0.0);
	return iso(p).normmult*normalize(clamp(
		
		(
		vec3(iso(p+e.xyy).d, iso(p+e.yxy).d, iso(p+e.yyx).d)
		//-vec3(iso(p-e.xyy).d, iso(p-e.yxy).d, iso(p-e.yyx).d)
		)
		 / e.x,
		
		vec3(-100.0),vec3(100.0)));
}

void main(void)
{
	vec2 uvc = uv - 1.0/texresolution;
	int mstep = 100;
	float perspective = 1.0;
	float inf = 1000.0;
	float ar = texresolution.y/texresolution.x;
	vec3 i = normalize(vec3(uvc.x-0.5,uvc.y-0.5, 1.0)*vec3(2.0,2.0*ar,1.0))*camerarot;
	vec2 uvo = uv;
	uvo += 1.0/texresolution;
  	vec3 ib = normalize(vec3(uvo.x-0.5,uvo.y-0.5, 1.0)*vec3(2.0,2.0*ar,1.0))*camerarot;
	vec3 v = i;
	int s = 0;
	
	vec3 p = v;
	p *= mix(0.0, texture2D(depthin, uvc).x, usedepthin);
	isodata d;
	float angle = length(i-ib);
	float isinf = 0.0;
	vec3 rp = p-camerapos*0.001;
	float stepsize = 0.5;
	if(texresolution.x-resolution.x > -0.5){
		angle = 0.0;
		stepsize = 0.5;
	}
	while(s<mstep){
		d = iso(rp);
		if(abs(d.d)-abs(angle*length(p)*2.0)<=0.0001 || abs(d.d)>=inf){
			d.color += d.d*vec3(100.0,0.0,0.0)/inf;
			if(abs(d.d)>=inf) isinf = 1.0;
			break;
		}
		p += v * d.d*stepsize;
		rp = p - camerapos*0.001;
		s++;
	}
	vec4 color = vec4(0.0);
	if(depthout < 0.5)
		color = vec4(dot(g(rp,1),vec3(1.0,0.0,0.0)))*10.0;// + vec4(0,0,s,0);
	color = clamp(color, 0.0,100.0);
	if(isinf > 0.5) color = vec4(100.0,0.0,0.0,1.0);
	c = mix(color, vec4(length(p)), depthout);
	//c = vec4(40.0);
}
