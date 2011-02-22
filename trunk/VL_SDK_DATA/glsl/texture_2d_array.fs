#extension GL_EXT_texture_array: enable
uniform sampler2DArray sampler0;
void main(void)
{
	gl_FragColor = texture2DArray(sampler0, gl_TexCoord[0].xyz );
}