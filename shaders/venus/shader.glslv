/*
	This shader does nothing but pass values to the fragment shader.
*/

uniform float time;

/*
	for fragment shader
*/
varying vec4 frag_pos;
varying float frag_len;
varying vec3 N;
varying vec3 v;

float SCALE = 0.0001 / usc;

void main() 
{
	vec3 pos;

	gl_FrontColor = gl_Color;

	pos = gl_Vertex.xyz;

	frag_len = length(pos);
	frag_pos = vec4(pos.x, pos.y, pos.z, 1.0);

	/*
		for lighting, push values to fragment shader:
	*/
	v = vec3(gl_ModelViewMatrix * gl_Vertex);       
	N = normalize(gl_NormalMatrix * gl_Normal);

	gl_Position	= gl_ProjectionMatrix * gl_ModelViewMatrix * frag_pos;
}
