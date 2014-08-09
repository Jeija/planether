#define MERCURY_RADIUS 2439.7
#define CRATER_NUM 100
#define CRATER_MINRADIUS 30.0
#define CRATER_MAXRADIUS 400.0
#define CRATER_MINTHICKNESS 7.0
#define CRATER_MAXTHICKNESS 14.0
#define CRATER_INTENSITY 0.01

/*
	for fragment shader
*/
varying vec4 frag_pos;
varying vec3 N;
varying vec3 v;

struct Crater
{
	vec3 position;
	float radius;
	float thickness;
};

// Will be updated every time a crater is generated
vec2 randseed = vec2(seed, seed);

Crater getCrater()
{
	// Generate some random values
	float x = rand(randseed + vec2(0.1, 0.0)) * 2.0 - 1.0;
	float y = rand(randseed + vec2(0.1, 0.1)) * 2.0 - 1.0;
	float z = rand(randseed + vec2(0.5, 0.4)) * 2.0 - 1.0;
	float r = rand(randseed + vec2(0.2, 0.8)) *
		(CRATER_MAXRADIUS - CRATER_MINRADIUS) + CRATER_MINRADIUS;
	float t = rand(randseed + vec2(0.1, 0.9)) *
		(CRATER_MAXTHICKNESS - CRATER_MINTHICKNESS) + CRATER_MINTHICKNESS;
	randseed = vec2(x + 0.5, t + 0.111);

	// Feed random values into crater properties
	Crater c;
	c.position = normalize(vec3(x, y, z)) * MERCURY_RADIUS;
	c.radius = r;
	c.thickness = t;

	return c;
}

void main() 
{
	frag_pos = gl_Vertex;
	vec3 pos = gl_Vertex.xyz / usc;
	float heightmult = 1.0;

	// Generate a list of crater positions. They will be the same every time.
	for (int i = 0; i <= CRATER_NUM; i++)
	{
		Crater c = getCrater();

		float dist = length(pos - c.position);

		if (	dist < c.radius + c.thickness &&
			dist > c.radius - c.thickness )
		{
			heightmult += CRATER_INTENSITY;
		}

		if (dist < c.radius - c.thickness) // inside
		{
			heightmult -= 0.04 * ((c.radius - dist) / c.radius);
		}
	}

	frag_pos.xyz = frag_pos.xyz * heightmult;

	/*
		for lighting, push values to fragment shader:
	*/
	v = vec3(gl_ModelViewMatrix * gl_Vertex);       
	N = normalize(gl_NormalMatrix * gl_Normal);

	gl_Position	= gl_ProjectionMatrix * gl_ModelViewMatrix * frag_pos;
}
