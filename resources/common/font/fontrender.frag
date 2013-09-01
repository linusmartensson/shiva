
#version 150

out vec4 out_Color;
in vec2 uv;
uniform sampler2D glyph;
uniform vec2 pos;
uniform vec2 size;

void main(void)
{
	vec2 uu = (uv-pos+vec2(0.f,0.3f))/size;
	vec4 f = texture(glyph, uu) * float(uu.x<1) * float(uu.y<1) * float(uu.x>0) * float(uu.y>0);
	out_Color = vec4(f.r,f.r,f.r,f.r);
}
