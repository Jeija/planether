#define SUNLIGHT_ID 1

varying vec3 N;
varying vec3 v;

vec4 getLight (vec4 color)
{
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
	vec4 color = vec4(0.3, 0.3, 0.3, 1.0);

	float stone = (snoise3d(frag_pos.xyz / usc * 0.002)) * 0.8 + 0.8;
	color *= vec4(stone, stone, stone, 1.0);

	float oct2 = (snoise3d(frag_pos.xyz / usc * 0.02)) * 0.8 + 0.8;
	color *= vec4(oct2, oct2, oct2, 1.0);
	color += vec4(0.1, 0.1, 0.1, 1.0);

	color = getLight(color);

	gl_FragColor = color;
}
