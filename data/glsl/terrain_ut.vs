uniform sampler2D heightmap_tex;
uniform mat4 matrix;
void main(void)
{
	mat4 m = gl_ModelViewProjectionMatrix * matrix;
	vec4 vertpos = gl_Vertex;
	vertpos.y = texture2D(heightmap_tex, gl_MultiTexCoord2.st).x * 10.0;
	gl_Position = m * vertpos;
	gl_TexCoord[0] = gl_MultiTexCoord0; // terrain texture
	gl_TexCoord[1] = gl_MultiTexCoord1; // detail texture
}
