varying vec4 frag_pos;

/*
	Nothing special here, just providing information for fragment shader
*/
void main() 
{
	frag_pos = gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_Position	= gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
