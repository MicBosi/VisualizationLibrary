uniform sampler2D heightmap_tex;
uniform float Height;
void main(void)
{
	vec4 vertpos = gl_Vertex;
	vertpos.y = texture2D(heightmap_tex, gl_MultiTexCoord2.st ).x * Height;
	gl_Position = gl_ModelViewProjectionMatrix * vertpos;
	gl_TexCoord[0] = gl_MultiTexCoord0; // terrain texture
	gl_TexCoord[1] = gl_MultiTexCoord1; // detail texture
}
