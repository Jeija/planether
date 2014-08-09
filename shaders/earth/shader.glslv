#define INT 0.1 // intensity of height difference

uniform float time;
uniform vec3 earth_pos;

/*
	for fragment shader
*/
varying vec4 frag_pos;
varying float frag_len;
varying vec3 N;
varying vec3 v;

float SCALE = 0.0001 / usc;

void main() 
{
	vec3 pos;

	gl_FrontColor = gl_Color;

	pos = gl_Vertex.xyz;

	vec3 noisepos_oct1 = pos / 100.0; // "continent" octave

	float heightmult = 1.0 + (snoise3d(noisepos_oct1)) * INT;

	if (heightmult > 1.0) // only at land
	{
		vec3 noisepos_oct2 = pos; // land octave / "hills"
		heightmult += (snoise3d(noisepos_oct2)) * INT / 80.0; // only upwards
	}

	if (heightmult > 1.0 + INT * 0.4) // only at high areas
	{
		vec3 noisepos_oct3 = pos * 1.0; // "mountain" octaves
		vec3 noisepos_oct4 = pos * 4.0; // "mountain" octaves
		vec3 noisepos_oct5 = pos * 8.0; // "mountain" octaves
		heightmult += (snoise3d(noisepos_oct3)) * INT / 20.0; // only upwards
		heightmult += (snoise3d(noisepos_oct4) - 0.5) * INT / 40.0;
		heightmult += (snoise3d(noisepos_oct5) - 0.5) * INT / 50.0;
	}

	if (heightmult > 1.0 - INT * 0.5) // ignore low areas (ocean)
	{
		vec3 noisepos_oct6 = pos / 5.0; // "spit" octave
		heightmult += (snoise3d(noisepos_oct6)) * INT / 5.0;
	}

	if (heightmult < 1.0 - INT * 0.1) // ocean: remove hills
	{
		heightmult -= (heightmult - (1.0 - INT * 0.1)) * 1.0;
	}

	pos = pos * heightmult;
	frag_len = length(pos);

	frag_pos = vec4(pos.x, pos.y, pos.z, 1.0);

	/*
		for lighting, push values to fragment shader:
	*/
	v = vec3(gl_ModelViewMatrix * gl_Vertex);       
	N = normalize(gl_NormalMatrix * gl_Normal);

	gl_Position = gl_ModelViewProjectionMatrix * frag_pos;
}
