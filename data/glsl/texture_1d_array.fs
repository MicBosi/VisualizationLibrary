#extension GL_EXT_texture_array: enable
uniform sampler1DArray sampler0;
void main(void)
{
	gl_FragColor = texture1DArray(sampler0, gl_TexCoord[0].xy );
}