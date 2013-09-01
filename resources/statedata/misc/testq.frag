#version 150
out vec4 color;
in vec2 uv;
uniform float t;

/*void main(void) {
	float x = 2.0*uv.x-1.0;
	float y = 2.0*uv.y-1.0;
	float r = atan(x,y);
	float d = sqrt(x*x+y*y)*2.0;
	color = vec4(
		cos((r)*12)>0.95&&d>0.8&&d<1.0?1.0:0.0,
		d<1.0&&cos((r)*60)-0.2>0.6&&d>0.95?1.0:(d>1.0&&d<1.01?1.0:0.0),
		(d-0.5),
		1.0)*
		(1.0-(sin(r*3+sin(d*13)+cos(r*7-t*0.3+sin(-r*11-1*d-t*0.4)+sin(d*2+r*2-1.0)*4.0)*1.0)*0.7+d*1.5-2.0));
	color = clamp(color.bbba,vec4(0.0),vec4(1.0))*1.5;
	color *= vec4(cos(r-t*0.23+sin(r+t*0.5)*2+d*2+cos(d)*2),
				 cos(r+sin(r*2+t*0.27-d)-t*0.12),
				 d*0.4,
				 1.0);
	color *= abs(color.r+color.g+color.b+1.0);
}*/

float sat(float a){
	return a>0.0?1.0:0.0;
}
/*
void main(void) {
	float x = 2.0*uv.x-1.0;
	float y = 2.0*uv.y-1.0;
	float xa = x+sin(t*0.21);
	float ya = y+cos(t*0.27+2);
	float ra = atan(xa,ya);
	float da = sqrt(xa*xa+ya*ya);
	float xb = x+sin(t*0.3);
	float yb = y+cos(t*0.25);
	float rb = atan(xb,yb);
	float db = sqrt(xb*xb+yb*yb);
	float xc = x+sin(t*0.42+1);
	float yc = y+cos(t*0.49+7);
	float rc = atan(xc,yc);
	float dc = sqrt(xc*xc+yc*yc);
	color = vec4(sat(
		cos((
		cos(ra*3-da*10.0+t+x*4+y*4)
	   *sin(rb*3+db*10.0+t+y*40)
	   *cos(rc*3+dc*10.0+t+x*40)
	   )*5*clamp(da*db*dc,0.0,1.0))
	   ));

	color = vec4(1.0-color.r,color.r,color.r,1.0);
}
*/
/*
void main(void) {
	float x = 2.0*uv.x-1.0;
	float y = 2.0*uv.y-1.0;
	float xa = x;
	float ya = y;
	float ra = atan(xa,ya);
	float da = sqrt(xa*xa+ya*ya);
	float xb = x+sin(t*0.3)*0.1;
	float yb = y+cos(t*0.3)*0.1;
	float rb = atan(xb,yb);
	float db = sqrt(xb*xb+yb*yb);
	float xc = x+sin(t*0.3+3.14159)*0.15;
	float yc = y+cos(t*0.3+3.14159)*0.15;
	float rc = atan(xc,yc);
	float dc = sqrt(xc*xc+yc*yc);
	color = vec4(sat(
		
		min(min(abs(da-0.95)-0.05,
		abs(db-0.5)-0.05),
		abs(dc-0.8)-0.05)
	   ),.0,0.0,da);
	color += dot(vec4(
		cos((ra)*12)>0.95&&da>0.8&&da<1.0?1.0:0.0,
		da<1.0&&cos((ra)*60)-0.2>0.6&&da>0.95?1.0:(da>1.0&&da<1.01?1.0:0.0),
		da*0.3,
		1.0)-(abs(db-0.5)*abs(dc-0.5)), vec4(1.0,1.0,0.25,0.0));
	color.rgb += 0.4*sin(color.r*20.0);
}*/
/*
void main(void) {
	float x = 2.0*uv.x-1.0;
	float y = 2.0*uv.y-1.0;
	float xa = x;
	float ya = y;
	float ra = atan(xa,ya);
	float da = sqrt(xa*xa+ya*ya);
	float xb = x+sin(t*0.3)*0.1;
	float yb = y+cos(t*0.3)*0.1;
	float rb = atan(xb,yb);
	float db = sqrt(xb*xb+yb*yb);
	float xc = x+sin(t*0.3+3.14159)*0.15;
	float yc = y+cos(t*0.3+3.14159)*0.15;
	float rc = atan(xc,yc);
	float dc = sqrt(xc*xc+yc*yc);
	color = vec4(sat(
		abs(ya-pow(cos(xc*30.0-xa*33.0),3.0)*1.0+sin(xc*5.0+t))-0.2
	   ));

}*/
/*
void main(void) {
	float x = 2.0*uv.x-1.0+t;
	float y = 2.0*uv.y-1.0+sin(x)*0.2;
	float ya = y*cos(x*10.0)*2.0;
	float xa = pow(sin(x*10.0),2);
	
	float ra = atan(xa,ya);
	float da = sqrt(xa*xa+ya*ya);
	float xb = sin(ra)*da;
	float yb = cos(ra)*da*2.5+cos(ra*9+da*70)*0.2;
	float rb = atan(xb,yb);
	float db = sqrt(xb*xb+yb*yb);
	float xc = x+sin(t*0.3+3.14159)*0.15;
	float yc = y+cos(t*0.3+3.14159)*0.15;
	float rc = atan(xc,yc);
	float dc = sqrt(xc*xc+yc*yc);
	
	color = vec4(sat(
		abs(da*1.3+abs(sqrt(xa*sqrt(xa)))+yb*0.4)-0.4)
		*sat(abs(y+sin(x*10.0-3.14159/2)*0.2)-0.05));

}*/

vec4 leaf(float aaa, float bbb, float tt){
	float x = 2.0*aaa-1.0+tt;
	float y = 2.0*bbb-1.0;
	float ya = y*2.0+1.0;
	float xa = pow(sin(x*10.0),2);
	
	float yam = ya+cos(xa*ya+2.8+atan(xa,y)*3.0*sin(x*3.14159)+x*37.0+sin(x*12.0));

	float ra = atan(xa,ya);
	
	float da = sqrt(xa*xa+ya*ya);
	float xb = sin(ra)*da;
	float yb = cos(ra)*da;
	float ybm = yb+cos(x+ra*2*cos(x*11.0*3.14159/2.0)+da*sin(x*3.14159*2.0)*3.79+x*16.0+sin(x*17.0))-0.2;
	float rb = atan(xb,yb);
	float db = sqrt(xb*xb+ybm*ybm);
	float xc = x+sin(t*0.3+3.14159)*0.15;
	float yc = y+cos(t*0.3+3.14159)*0.15;
	float rc = atan(xc,yc);
	float dc = sqrt(xc*xc+yc*yc);
	
	vec4 cc = vec4(
		(1.0-sat(abs(db*1.2+atan(yam,xa+sin(x))+ra*10.0)-3.0))*(cos(x*7.0)),
		(1.0-sat(abs(db*1.3+rb*2.1)-1.8))*sin(db+rb+ybm*sin(x*1.0))*2.0-1.0,
		0.0,1.0).rgba;
	cc.b += cc.r+cc.g*2.0;
	return cc;
}

vec4 flower(float x, float y, float t){
	return max(leaf(x,y, t),0.0)+max(leaf(x,y+0.2, t+1.25),0.0)+max(leaf(x,y+0.1, t+2.2),0.0);
}

void main(void) {
	float x = uv.x;
	float ax = (abs(uv.x*2.0-1.0));
	ax *= ax*ax*ax*ax*ax;
	x *= 2.0;
	float y = uv.y*(1.0+ax);
	color = flower(x,y,0.9)+flower(x*1.3,y*1.7,10.5)+flower(x*1.3,y*3.0,19.1);
	color *= clamp(1.0-ax,0.0,1.0);
	color = vec4(color.r*0.4+color.b*0.4,color.r*color.g,color.r*0.5,color.r*color.g*color.b).grba;
}