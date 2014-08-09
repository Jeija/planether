#define INT 0.04 // intensity of height difference
#define CRATER_INT 0.04

uniform float time;

/*
	for fragment shader
*/
varying vec4 frag_pos;
varying vec3 N;
varying vec3 v;

float SCALE = 0.0001 / usc;

float getCraterNoise(vec3 pos, float scale)
{
	return (sin(pos.x / usc * scale) + sin(pos.y / usc * scale) + sin(pos.z / usc * scale)) / 3.0;
}

void main() 
{
	vec3 pos;

	gl_FrontColor = gl_Color;

	pos = gl_Vertex.xyz;

	vec3 noisepos_oct1 = pos / 2.0; // "continent" octave
	vec3 noisepos_oct2 = pos * 8.0; // "spit" octave

        float heightmult = 1.02;

	heightmult += (snoise3d(noisepos_oct1)) * INT;
	heightmult += (snoise3d(noisepos_oct2)) * INT / 20.0;

	// Large craters:
	float largecrater = getCraterNoise(pos, 0.01);
	if (largecrater > 0.0)
	{
		heightmult = pow(heightmult, 0.5);
		heightmult -= largecrater * CRATER_INT;
	}

	// Small craters:
	float smallcrater = getCraterNoise(pos, 0.1);
	if (smallcrater > 0.4)
	{
		heightmult = pow(heightmult, 0.9);
		heightmult -= smallcrater * CRATER_INT * 0.3;
	}

	// Medium, but intense craters:
	float mediumcrater = getCraterNoise(pos, 0.03);
	if (mediumcrater > 0.3)
	{
		heightmult = pow(heightmult, 0.8);
		heightmult -= mediumcrater * CRATER_INT;
	}

	pos = pos * heightmult;

	frag_pos = vec4(pos.x, pos.y, pos.z, 1.0);

	/*
		for lighting, push values to fragment shader:
	*/
	v = vec3(gl_ModelViewMatrix * gl_Vertex);       
	N = normalize(gl_NormalMatrix * gl_Normal);

	gl_Position	= gl_ProjectionMatrix * gl_ModelViewMatrix * frag_pos;
}
