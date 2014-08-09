#define INT 0.04 // intensity of height difference

uniform float time;

/*
	for fragment shader
*/
varying vec4 frag_pos;
varying float frag_len;
varying vec3 N;
varying vec3 v;

void main() 
{
	vec3 pos;

	gl_FrontColor = gl_Color;

	pos = gl_Vertex.xyz;

	vec3 noisepos_oct1 = pos / 20.0; // larger octave
	vec3 noisepos_oct2 = pos /  5.0; // smaller octave
	vec3 noisepos_oct3 = pos *  2.0; // tiny octave

	float heightmult = 1.0 + (snoise3d(noisepos_oct1)) * INT * 0.7;
	heightmult += snoise3d(noisepos_oct2) * INT * 0.4;
	heightmult += snoise3d(noisepos_oct3) * INT * 0.2;

	pos = pos * heightmult;
	frag_len = length(pos);

	pos = pos;
	frag_pos = vec4(pos.x, pos.y, pos.z, 1.0);

	/*
		for lighting, push values to fragment shader:
	*/
	v = vec3(gl_ModelViewMatrix * gl_Vertex);       
	N = normalize(gl_NormalMatrix * gl_Normal);

	gl_Position	= gl_ProjectionMatrix * gl_ModelViewMatrix * frag_pos;
}
