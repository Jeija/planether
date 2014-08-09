varying vec3 frag_pos;
varying vec3 worldpos;

void main()
{
	frag_pos = gl_Vertex.xyz;
	worldpos = vec4(gl_Vertex * gl_ModelViewMatrix).xyz;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
