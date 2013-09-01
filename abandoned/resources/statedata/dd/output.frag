
#version 150

out vec4 texOut;
uniform float aspect;
uniform float time;
in vec2 uv;

float dc(vec3 pos){
	pos.x = mod(pos.x, 3.0);
	pos.z = mod(pos.z, 20.0);
	float s1 = distance(pos, vec3(1.0,sin(time)*10.0,14.0))-1.0;

	return s1;
}

void main(void)
{
	vec2 uu = uv+vec2(0.0,-0.3);
	texOut = time*time-pow(vec4(5.0)*length(uu-0.5),vec4(2.0));
}
