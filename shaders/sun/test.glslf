#define SPEED 0.8

uniform float time;
varying vec4 frag_pos;
vec4 pos;

float SCALE = 0.00001 / usc;

void main()
{
	pos = frag_pos * SCALE;
	vec3 offset;
	offset.x = cos(time / 100.0) * 8.5;
	offset.y = sin(time / 100.0) * 8.0;
	offset.z = sin(time / 100.0) * 8.5;

	vec3 screenpos = pos.xyz;

	float add = snoise3d(screenpos + offset);
	float r = 1.0;
	float g = 0.8 + add;
	float b = 0.2 + add;
	vec4 color = vec4(r, g, b, 1.0);

	gl_FragColor = color;
}
