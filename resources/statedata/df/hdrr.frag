
uniform sampler2D depth;
uniform sampler2D color;
uniform sampler2D colormap;
/*uniform mat4 camera;
uniform mat3 rot;
uniform vec3 trans;*/
in vec2 uv;
out vec4 texOut;


void main(void)
{
	vec2 iuv = uv;
	vec3 f = texture2D(depth,vec2(0.0,1.0)+uv.xy*vec2(1.0,-1.0)).rrr/4192.0;
    vec2 h = texture2D(colormap,vec2(0.0,1.0)+uv.xy*vec2(0.5,-1.0)).xy;
	
	vec3 g = texture2D(color,h/vec2(640.0,480.0)).rgb;
	
	texOut.w = 1.0;
	texOut.rgb=f.x>0.01&&f.x<1.0?g.rgb*0.2/f.xxx:vec3(0.0);

}