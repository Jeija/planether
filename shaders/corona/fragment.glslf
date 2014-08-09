varying vec3 frag_pos;
uniform float radius;
uniform float time;

void main()
{
	// Generate wreath - large octave
	vec3 wreath_pos = frag_pos / 10000.0;
	wreath_pos.z += time * 10.0 * usc;
	float wreath = (snoise3d(wreath_pos) + 10.0) / 11.0;

	// Generate wreath - small octave
	vec3 wreath_pos2 = frag_pos / 1000.0;
	wreath_pos2.z += time * 10.0 * usc;
	float wreath2 = (snoise3d(wreath_pos2) + 10.0) / 11.0;

	float corona_size = radius * 0.8;

	float len = length(frag_pos) - radius;
	float prop = (corona_size - len) / corona_size;

	vec4 color = vec4(1.0, 0.9, 0.6, 1.0) * prop * wreath * wreath2;
	if (color.a > 1.0) color.a = 1.0;

	color.a *= 0.8;

	gl_FragColor = color;
}
