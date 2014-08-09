#define SUNLIGHT_ID 1

varying vec3 N;
varying vec3 v;

varying vec4 color;

vec4 getLight (vec4 color)
{
	vec4 newcolor = vec4(0.0, 0.0, 0.0, 1.0) + color * 0.1;

	vec3 L = normalize(gl_LightSource[SUNLIGHT_ID].position.xyz - v);
	vec3 E = normalize(-v);
	vec3 R = normalize(-reflect(L,N));

	// calculate Diffuse Term:
	newcolor += clamp(color * max(dot(N,L), 0.0), 0.0, 1.0);

	newcolor.a = color.a;

	// write Total Color:
	return newcolor;
}

void main()
{
	vec4 lcolor = getLight(color);

	gl_FragColor = lcolor;
}
