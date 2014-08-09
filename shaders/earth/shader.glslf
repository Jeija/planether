#define EARTH_RADIUS 6371.0
#define INT 0.1 // intensity of height difference
#define SUNLIGHT_ID 1
#define EARTH_SHININESS 2.0

varying vec3 N;
varying vec3 v;
uniform int preview;

vec4 getLight (vec4 color)
{
	if (preview == 2) return color;
	vec4 newcolor = vec4(0.0, 0.0, 0.0, 1.0);
	newcolor += color * 0.03;

	vec3 L = normalize(gl_LightSource[SUNLIGHT_ID].position.xyz - v);
	vec3 E = normalize(-v);
	vec3 R = normalize(-reflect(L,N));

	// calculate Diffuse Term:
	newcolor += clamp(color * max(dot(N,L), 0.0), 0.0, 1.0);

	// calculate Specular Term:
	newcolor += clamp(newcolor * pow(max(dot(R,E),0.0), 0.3*EARTH_SHININESS), 0.0, 1.0) * color.b;

	// write Total Color:
	return newcolor;
}

uniform float time;
varying vec4 frag_pos;  // from Vertex
varying float frag_len; // Shader
vec4 pos;

float SCALE = 0.00001 / usc;

void main()
{
	// Error color: (red)
	vec4 color = vec4(1.0, 0.0, 0.0, 0.0);

	// Ocean <--> Deep Water
	if (color.r == 1.0)
	color = land_color_interp(EARTH_RADIUS, INT, frag_len, -50.0, 27.0,
			vec4(0.0, 0.1, 0.7, 1.0), vec4(0.0, 0.1, 0.5, 1.0));

	// Deep Water <--> Water
	if (color.r == 1.0)
	color = land_color_interp(EARTH_RADIUS, INT, frag_len, -13.0, 10.0,
			vec4(0.0, 0.2, 1.0, 1.0), vec4(0.0, 0.1, 0.7, 1.0));

	if (color.r != 1.0) // add some waves as snoise3d
	{
		color += snoise3d(frag_pos.xyz/usc/300.0 + time / 10.0) / 10.0;
	}

	// Water <--> Beach
	if (color.r == 1.0)
	color = land_color_interp(EARTH_RADIUS, INT, frag_len, -1.0, 2.0,
			vec4(0.67, 0.65, 0.1, 1.0), vec4(0.0, 0.2, 1.0, 1.0));

	// Beach <--> Land
	if (color.r == 1.0)
	color = land_color_interp(EARTH_RADIUS, INT, frag_len, 4.0, 3.0,
			vec4(0.0, 0.5, 0.05, 1.0), vec4(0.6, 0.65, 0.1, 1.0));

	// Land <--> Forest
	if (color.r == 1.0)
	color = land_color_interp(EARTH_RADIUS, INT, frag_len, 14.0, 7.0,
			vec4(0.0, 0.3, 0.0, 1.0), vec4(0.0, 0.5, 0.05, 1.0));

	// Forest <--> Mountain
	if (color.r == 1.0)
	color = land_color_interp(EARTH_RADIUS, INT, frag_len, 29.0, 8.0,
			vec4(0.7, 0.8, 0.8, 1.0), vec4(0.0, 0.3, 0.00, 1.0));

	// Mountain <--> Snow
	if (color.r == 1.0)
	color = land_color_interp(EARTH_RADIUS, INT, frag_len, 45.0, 8.0,
			vec4(0.95, 1.0, 1.0, 1.0), vec4(0.7, 0.8, 0.8, 1.0));

	// Snow <--> Upwards
	if (color.r == 1.0)
	color = land_color_interp(EARTH_RADIUS, INT, frag_len, 100.0, 47.0,
			vec4(0.95, 1.0, 1.0, 1.0), vec4(0.95, 1.0, 1.0, 1.0));

	color = getLight(color);

	// Night = City lights
	if (color.b < 0.05 && color.g < 0.05) // at night
	{
		float height_real	= EARTH_RADIUS * usc + 6.0 * INT;
		float divergence_real	= 6.0 * INT;
		// must be above sea level, but not too high to be a city
		if (is_near(height_real, divergence_real, frag_len))
		{
			if (snoise3d(frag_pos.xyz / 1.0) > 0.2)
			{
				if (snoise3d(frag_pos.xyz * 100.0) > 0.5)
				{
					float r = 0.9 + snoise3d(frag_pos.xyz * 20.0) * 0.1;
					float g = 0.7 + snoise3d(frag_pos.xyz * 50.0) * 0.3;
					float b = 0.4 + snoise3d(frag_pos.xyz * 80.0) * 0.3;
					color = vec4(r, g, b, 1.0);
				}
			}
		}
	}

	gl_FragColor = color;
}
