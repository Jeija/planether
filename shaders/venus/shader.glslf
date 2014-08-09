#define INT 0.04 // intensity of height difference
#define SUNLIGHT_ID 1

varying vec3 N;
varying vec3 v;
uniform int preview;

vec4 getLight (vec4 color)
{
	if (preview == 2) return color;
	vec4 newcolor = vec4(0.0, 0.0, 0.0, 1.0);
	newcolor += color * 0.08;

	vec3 L = normalize(gl_LightSource[SUNLIGHT_ID].position.xyz - v);
	vec3 E = normalize(-v);
	vec3 R = normalize(-reflect(L,N));

	// calculate Diffuse Term:
	newcolor += clamp(color * max(dot(N,L), 0.0), 0.0, 1.0);

	// write Total Color:
	return newcolor;
}

uniform float time;
varying vec4 frag_pos;  // from Vertex
vec4 pos;

void main()
{
	// Plain color: yellow
	vec4 color = vec4(0.4, 0.4, 0.1, 0.0);
	vec3 cpos = frag_pos.xyz;

	// clouds along the latitudes
	vec3 cloudnoise1 = cpos * 9.0 * usc;
	color *= (snoise3d(cloudnoise1) + 1.0) * vec4(1.5, 1.0, 1.0, 1.0);

	vec3 cloudnoise2 = cpos * 3.0 * usc;
	color *= (snoise3d(cloudnoise2) + 1.0) * vec4(3.0, 2.0, 1.0, 1.0);

	vec3 cloudnoise3 = cpos * 7.0 * usc;
	color *= (snoise3d(cloudnoise2) + 1.0) * vec4(0.8, 0.6, 1.0, 1.0);

	color = sqrt(color);
	color += vec4(0.2, 0.2, 0.1, 0.0);

	color.a = 1.0;
	gl_FragColor = getLight(color);
}
