#define SPEED 0.5
#define INT 1.1 // intensity

uniform float time;
vec4 growvert;
varying vec4 frag_pos;
vec4 pos;

float SCALE = 0.0001 / usc;

void main() 
{
	gl_FrontColor = gl_Color;
	vec4 offset;
	offset.x = cos(time) / 5.0;
	offset.y = sin(time) / 5.0;
	offset.z = cos(time) / 5.0;
	offset.w = 0.0;
	pos = gl_Vertex * SCALE * 0.2 + offset;
	vec3 pos3 = pos.xyz;

	float add = (snoise3d(pos3) - 0.5) / 50.0 + 0.5;
	growvert = gl_Vertex * vec4(add, add, add, 1.0);
	frag_pos = growvert;
	gl_Position	= gl_ProjectionMatrix * gl_ModelViewMatrix * growvert;
}
