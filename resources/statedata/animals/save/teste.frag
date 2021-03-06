
#version 330
vec3 permute(vec3 x)
{
  return mod(((x*34.0)+1.0)*x, 289.0);
}

vec4 permute(vec4 x)
{
  return mod(((x*34.0)+1.0)*x, 289.0);
}

float permute(float x)
{
  return floor(mod(((x*34.0)+1.0)*x, 289.0));
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float taylorInvSqrt(float r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 grad4(float j, vec4 ip)
  {
  const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
  vec4 p,s;

  p.xyz = floor( fract (vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
  p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
  s = vec4(lessThan(p, vec4(0.0)));
  p.xyz = p.xyz + (s.xyz*2.0 - 1.0) * s.www; 

  return p;
  }

//
// Description : Array and textureless GLSL 2D/3D/4D simplex 
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110410 (stegu)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//

float snoise(vec4 v)
  {
  const vec4  C = vec4( 0.138196601125011,  // (5 - sqrt(5))/20  G4
                        0.276393202250021,  // 2 * G4
                        0.414589803375032,  // 3 * G4
                       -0.447213595499958); // -1 + 4 * G4
						
// (sqrt(5) - 1)/4 = F4, used once below
#define F4 0.309016994374947451

// First corner
  vec4 i  = floor(v + dot(v, vec4(F4)) );
  vec4 x0 = v -   i + dot(i, C.xxxx);

// Other corners

// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
  vec4 i0;
  vec3 isX = step( x0.yzw, x0.xxx );
  vec3 isYZ = step( x0.zww, x0.yyz );
//  i0.x = dot( isX, vec3( 1.0 ) );
  i0.x = isX.x + isX.y + isX.z;
  i0.yzw = 1.0 - isX;
//  i0.y += dot( isYZ.xy, vec2( 1.0 ) );
  i0.y += isYZ.x + isYZ.y;
  i0.zw += 1.0 - isYZ.xy;
  i0.z += isYZ.z;
  i0.w += 1.0 - isYZ.z;

  // i0 now contains the unique values 0,1,2,3 in each channel
  vec4 i3 = clamp( i0, 0.0, 1.0 );
  vec4 i2 = clamp( i0-1.0, 0.0, 1.0 );
  vec4 i1 = clamp( i0-2.0, 0.0, 1.0 );

  //  x0 = x0 - 0.0 + 0.0 * C.xxxx
  //  x1 = x0 - i1  + 0.0 * C.xxxx
  //  x2 = x0 - i2  + 0.0 * C.xxxx
  //  x3 = x0 - i3  + 0.0 * C.xxxx
  //  x4 = x0 - 1.0 + 4.0 * C.xxxx
  vec4 x1 = x0 - i1 + C.xxxx;
  vec4 x2 = x0 - i2 + C.yyyy;
  vec4 x3 = x0 - i3 + C.zzzz;
  vec4 x4 = x0 + C.wwww;

// Permutations
  i = mod(i, 289.0); 
  float j0 = permute( permute( permute( permute(i.w) + i.z) + i.y) + i.x);
  vec4 j1 = permute( permute( permute( permute (
             i.w + vec4(i1.w, i2.w, i3.w, 1.0 ))
           + i.z + vec4(i1.z, i2.z, i3.z, 1.0 ))
           + i.y + vec4(i1.y, i2.y, i3.y, 1.0 ))
           + i.x + vec4(i1.x, i2.x, i3.x, 1.0 ));

// Gradients: 7x7x6 points over a cube, mapped onto a 4-cross polytope
// 7*7*6 = 294, which is close to the ring size 17*17 = 289.
  vec4 ip = vec4(1.0/294.0, 1.0/49.0, 1.0/7.0, 0.0) ;

  vec4 p0 = grad4(j0,   ip);
  vec4 p1 = grad4(j1.x, ip);
  vec4 p2 = grad4(j1.y, ip);
  vec4 p3 = grad4(j1.z, ip);
  vec4 p4 = grad4(j1.w, ip);

// Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;
  p4 *= taylorInvSqrt(dot(p4,p4));

// Mix contributions from the five corners
  vec3 m0 = max(0.6 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0);
  vec2 m1 = max(0.6 - vec2(dot(x3,x3), dot(x4,x4)            ), 0.0);
  m0 = m0 * m0;
  m1 = m1 * m1;
  return 49.0 * ( dot(m0*m0, vec3( dot( p0, x0 ), dot( p1, x1 ), dot( p2, x2 )))
               + dot(m1*m1, vec2( dot( p3, x3 ), dot( p4, x4 ) ) ) ) ;

  }
//
// Description : Array and textureless GLSL 2D/3D/4D simplex 
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110410 (stegu)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//


float snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod(i, 289.0 ); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  }
//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110410 (stegu)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//


float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod(i, 289.0); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Inlined for speed: m *= taylorInvSqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}


layout(location=0) out vec4 w;
layout(location=1) out vec4 a;
layout(location=2) out vec4 c;
in vec2 uv;
uniform mat4 camera;
uniform float time;
uniform mat3 rot;
uniform vec3 trans;
uniform float skip;


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
	

	float fa = 0.0;
	float fk = 0.0;
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
	c = cmod(ray,fa,fk,aa,ca,da,ga,dir,dm,k,tf,dp,step).bgga*200.0;
	w = vec4(ray+dir*step*dm, 1.0);
	a = vec4(sin(w.x)*9200.0,sin(w.y)*92200.0,sin(w.z)*19200.0,sin(dot(w,w))+2.0);
}
