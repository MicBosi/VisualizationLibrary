// Simply passes the vertex frag_position and texture coordinate to the fragment shader. 
// It also passes the vertex coord in object space to perform per-pixel lighting.

varying vec3 frag_position; // in object space

void main(void)
{
	frag_position = gl_Vertex.xyz;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}

// Have fun!