#define INT 0.04 // intensity of height difference
#define SUNLIGHT_ID 1
#define SAT_SHININESS 3.0

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
	newcolor += clamp(newcolor * pow(max(dot(R,E),0.0), 0.3*SAT_SHININESS), 0.0, 1.0) * color.b;

	// write Total Color:
	return newcolor;
}

uniform float time;
varying vec4 frag_pos;  // from Vertex
vec4 pos;

void main()
{
	// Plain color: blue-white
	vec4 color = vec4(0.5, 0.5, 0.9, 0.0);

	// clouds along the latitudes
	vec3 cloudnoise1 = vec3(frag_pos.x / 30.0, frag_pos.y, frag_pos.z / 30.0) / 10.0 * usc;
	color *= (snoise3d(cloudnoise1) + 1.0) * vec4(1.0, 1.0, 1.5, 1.0);

	vec3 cloudnoise2 = vec3(frag_pos.x / 10.0, frag_pos.y, frag_pos.z / 10.0) /  2.0 * usc;
	color *= (snoise3d(cloudnoise2) + 1.0) * vec4(0.8, 0.8, 1.2, 1.0);

	vec3 cloudnoise3 = vec3(frag_pos.x / 10.0, frag_pos.y, frag_pos.z / 10.0) *  2.0 * usc;
	color *= (snoise3d(cloudnoise2) + 1.0) * vec4(1.1, 1.1, 1.0, 1.0);

	color = sqrt(color);
	color += vec4(0.2, 0.2, 0.1, 0.0);

	color.a = 1.0;
	gl_FragColor = getLight(color);
}
