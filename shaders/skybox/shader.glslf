varying vec4 frag_pos;
uniform sampler2D my_color_texture;

void main()
{
	vec4 color = texture2D(my_color_texture, gl_TexCoord[0].xy);

	/* Exagerrate special properties of the stars */
	float avgcol = (color.r + color.g + color.b) / 3.0;
	color.r *= (avgcol - color.r) *  8.0 + 1.0;
	color.g *= (avgcol - color.g) *  4.0 + 1.0;
	color.b *= (avgcol - color.b) *  2.0 + 1.0;

	color *= (rand(frag_pos.xy * frag_pos.xz) + 1.0) * 1.4 - 1.5;

	gl_FragColor = color;
}
