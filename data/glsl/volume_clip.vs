varying vec4 WorldPosition;
uniform mat4 TextureFrame;
void main(void)
{
	WorldPosition  = TextureFrame * vec4(gl_MultiTexCoord0.xyz, 1.0);
	gl_Position    = ftransform();
	gl_FrontColor  = gl_Color;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
