/*
	for fragment shader
*/
varying vec3 N;
varying vec3 v;

varying vec4 color;

void main() 
{
	/*
		for lighting, push values to fragment shader:
	*/
	v = vec3(gl_ModelViewMatrix * gl_Vertex);       
	N = normalize(gl_NormalMatrix * gl_Normal);

	color = gl_Color;

	gl_Position	= gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
