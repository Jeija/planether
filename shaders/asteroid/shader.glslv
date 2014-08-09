uniform float scale;

/*
	for fragment shader
*/
varying vec4 frag_pos;
varying vec3 N;
varying vec3 v;

void main() 
{
	/*
		for lighting, push values to fragment shader:
	*/
	v = vec3(gl_ModelViewMatrix * gl_Vertex);       
	N = normalize(gl_NormalMatrix * gl_Normal);

	frag_pos = gl_Vertex;
	frag_pos.x *= scale; 
	frag_pos.y *= scale;
	frag_pos.z *= scale;


	frag_pos.xyz *= rand(frag_pos.xy * frag_pos.z) * 0.8 + 0.6;

	gl_Position	= gl_ProjectionMatrix * gl_ModelViewMatrix * frag_pos;
}
