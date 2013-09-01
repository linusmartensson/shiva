#version 330

uniform sampler2D color;
uniform sampler2D depth;
uniform sampler2D wform;
uniform float time;

uniform vec4 res;

in vec2 uv;
out vec4 c;

void main(){

	vec2 uvi = uv;

	float f = smoothstep(25.0,25.5,time)*(1.0-smoothstep(32.57,32.6,time));

	uvi.x += f*sin(time+uv.y*res.y*0.02+uv.y*res.y*0.1*sin(time*0.01))*sin(time)*0.01;

	uvi = uvi*2.0-1.0;

	float g = smoothstep(32.57,32.6, time)*(1.0-smoothstep(50.0,65.0,time));
	g *= (texture2D(wform, vec2(0.02,0.5))+texture2D(wform, vec2(0.1,0.5))+texture2D(wform, vec2(0.2,0.5)))*0.1;

	vec2 rd = vec2(atan(uvi.x,uvi.y), sqrt(dot(uvi,uvi)));

	rd.y -= f*(sin(rd.y*5.0+time)*0.1+0.1)*rd.y;

	rd.y /= 1.0+abs(g)*2.0;

	uvi.x = sin(rd.x)*rd.y;
	uvi.y = cos(rd.x)*rd.y;
	uvi = uvi*0.5+0.5;

	vec4 i = texture2D(color, uvi.xy);
	vec4 j = abs(i);
	
	float fac = 0.5;

	float mx = 10.0;
	float sd = 0.0;
	for(float i=1.f;i<mx;i+=1.f){
		j += abs(texture2D(color, uvi.xy+res.zw*i)*fac);
		j += abs(texture2D(color, uvi.xy-res.zw*i)*fac);
		j += abs(texture2D(color, uvi.xy+vec2(res.z,-res.w)*i)*fac);
		j += abs(texture2D(color, uvi.xy-vec2(res.z,-res.w)*i)*fac);
		
		sd = max(texture2D(color, uvi.xy+res.zw*i).r, sd);
		sd = max(texture2D(color, uvi.xy-res.zw*i).r,sd);
		sd = max(texture2D(color, uvi.xy+vec2(res.z,-res.w)*i).r,sd);
		sd = max(texture2D(color, uvi.xy-vec2(res.z,-res.w)*i).r,sd);
		fac *= 0.8;
	}
	j /= mx*0.2;
		
	i = abs(i);

	i /= (1.0+i);
	j /= (1.0+j);

	sd = 2.0*1.0*5000.0/(sd*(5000.0-1.0)-(5000.0+1.0));
	sd = (-sd - 1.0) / (5000.0-1.0);
	sd *= 1000.0;
	sd = clamp(abs(sd),0.0,1.0);
	i = (i*(1.0-sd)+j*(sd));
	i *= 2.0;

	c = 
		vec4(
		i.r*i.r+cos(i.r)*i.r*3.5*0.5, 
		sin(i.r)+i.r*i.r*2.0*2.0, 
		i.r*2.0-i.r*i.r*2.0, 1.0
		);

	c = c*vec4(0.5,1.0,1.0,1.0)*(1.0-smoothstep(24.0,28.0,time)) + 
	vec4(i.r*3.0,i.r*0.1,i.r*0.1,1.0)*smoothstep(24.0,28.0, time)*(1.0-smoothstep(34.0,35.0, time))
	+(c)*vec4(3.0,sin(time*20.0)*0.2+1.5,1.0,0.2)*smoothstep(32.57,32.6,time);


	c = clamp(c, vec4(0.0),vec4(1.0));
}