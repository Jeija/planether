#define MOON_RADIUS 1737.0
#define INT 0.04 // intensity of height difference
#define SUNLIGHT_ID 1

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

	// no Specular Term

	// write Total Color:
	return newcolor;
}

uniform float time;
varying vec4 frag_pos;

void main()
{
	float frag_len = length(frag_pos.xyz);
	// Error color: (red)
	vec4 color = vec4(1.0, 0.0, 0.0, 0.0);

	// Valley <--> Deep Down
	if (color.r == 1.0)
	color = land_color_interp(MOON_RADIUS, INT, frag_len, -40.0, 34.0,
				vec4(0.28, 0.28, 0.28, 1.0), vec4(0.3, 0.3, 0.3, 1.0));
	// Average Height <--> Crater / Hill
	if (color.r == 1.0)
	color = land_color_interp(MOON_RADIUS, INT, frag_len, 0.0, 6.0,
				vec4(0.75, 0.75, 0.75, 1.0), vec4(0.3, 0.3, 0.3, 3.0));

	// Crater / Hill <--> Upwards
	if (color.r == 1.0)
	color = land_color_interp(MOON_RADIUS, INT, frag_len, 40.0, 34.0,
			vec4(0.85, 0.85, 0.85, 1.0), vec4(0.75, 0.75, 0.75, 1.0));

	// add some offset
	float offset = (snoise3d((frag_pos.xyz + vec3(time, 0.0, 0.0))
			* 80.0) - 0.5) / 20.0; // +time for fancy flickering
	offset += (snoise3d(frag_pos.xyz		   /  2.0) - 0.5) / 20.0;

	color += vec4(offset, offset, offset, 0);

	color = getLight(color);

	gl_FragColor = color;
}
