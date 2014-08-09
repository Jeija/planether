varying vec3 frag_pos;
varying vec3 worldpos;
uniform float size;
uniform float time;
uniform float exptime;

void main()
{
	float len = length(frag_pos);
	float prop = (size - len) / size;
	prop = prop * (1.0 - time / exptime);

	float redness = time / exptime;
	float blueness = 1.0 - redness;

	vec4 color = vec4(	redness * 1.5 + 0.1 * blueness,
				redness / 2.0 + 0.1 * blueness,
				blueness, prop);
	color *= vec4(	(snoise3d(worldpos / usc * 0.2) + 0.5) * 0.8 + 0.8,
			(snoise3d(worldpos / usc * 0.2) + 0.5) * 0.8 + 0.8,
			(snoise3d(worldpos / usc * 0.2) + 0.5) * 0.8 + 0.8, 1.0);

	gl_FragColor = color;
}
