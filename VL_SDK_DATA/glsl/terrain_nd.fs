uniform sampler2D terrain_tex;
void main(void)
{
	gl_FragColor = texture2D(terrain_tex, gl_TexCoord[0].st );
}
