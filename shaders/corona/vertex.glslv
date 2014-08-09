varying vec3 frag_pos;

void main()
{
	frag_pos = gl_Vertex.xyz;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
