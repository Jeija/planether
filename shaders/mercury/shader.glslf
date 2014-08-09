#define MERCURY_RADIUS 2439.7
#define SUNLIGHT_ID 1
#define CRATER_NUM 100
#define CRATER_INTENSITY 0.01

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

	vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

	// Crater Center (max. depth) <--> Down
	if (color.r == 1.0)
	color = land_color_interp(MERCURY_RADIUS, CRATER_INTENSITY, frag_len, -150.0, 90.0,
				vec4(0.25, 0.25, 0.25, 1.0), vec4(0.1, 0.1, 0.1, 1.0));

	// Down <--> Crater
	if (color.r == 1.0)
	color = land_color_interp(MERCURY_RADIUS, CRATER_INTENSITY, frag_len, -30.0, 30.0,
				vec4(0.3, 0.3, 0.3, 1.0), vec4(0.25, 0.25, 0.25, 1.0));
	// Crater --> Up
	if (color.r == 1.0)
	color = land_color_interp(MERCURY_RADIUS, CRATER_INTENSITY, frag_len, 50.0, 50.0,
				vec4(0.8, 0.8, 0.8, 1.0), vec4(0.3, 0.3, 0.3, 1.0));


	color *= snoise3d(frag_pos.xyz / 4.0) /  5.0 + 0.90;
	color *= snoise3d(frag_pos.xyz / 2.0) / 10.0 + 0.95;
	color *= snoise3d(frag_pos.xyz * 2.0) / 15.0 + 0.98;
	color.a = 1.0;

	gl_FragColor = getLight(color);
}
