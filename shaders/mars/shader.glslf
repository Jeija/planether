#define MARS_RADIUS 3390.0
#define INT 0.04 // intensity of height difference
#define SUNLIGHT_ID 1
#define MARS_SHININESS 2.0

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
	newcolor += clamp(newcolor * pow(max(dot(R,E),0.0), 0.3*MARS_SHININESS), 0.0, 1.0) * color.b;

	// write Total Color:
	return newcolor;
}

uniform float time;
varying vec4 frag_pos;  // from Vertex
varying float frag_len; // Shader
vec4 pos;

void main()
{
	// Error color: (red)
	vec4 color = vec4(1.0, 0.0, 0.0, 0.0);

	// Black <--> Rust
	if (color.r == 1.0)
	color = land_color_interp(MARS_RADIUS, INT, frag_len, -20.0, 20.0,
			vec4(0.95, 0.2, 0.1, 1.0), vec4(0.1, 0.0, 0.0, 1.0));

	// Rust <--> Red
	if (color.r == 1.0)
	color = land_color_interp(MARS_RADIUS, INT, frag_len,
			20.0, 20.0, vec4(0.95, 0.4, 0.1, 1.0), vec4(0.95, 0.2, 0.1, 1.0));

	// Snowy poles
	float pole_dist = (abs(frag_pos.y) - MARS_RADIUS * usc * 0.8);
	float maxheight_dist = frag_len - MARS_RADIUS * usc - 0.1 * INT;

	if (pole_dist > 0.0)
	{
		color += vec4(1.0, 1.0, 1.0, 1.0) * (pole_dist) / 2.0 * maxheight_dist;
		color.a = 1.0;
	}

	color = getLight(color);

	// Alien settlements
	if (snoise3d(frag_pos.xyz) > 0.95)
		color = vec4(0.0, 1.0, 0.0, 1.0);

	color.a = 1.0;
	gl_FragColor = color;
}
