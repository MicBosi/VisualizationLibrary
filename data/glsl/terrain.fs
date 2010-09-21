uniform sampler2D terrain_tex;
uniform sampler2D detail_tex;
void main(void)
{
	gl_FragColor = texture2D(terrain_tex, gl_TexCoord[0].st ) * texture2D(detail_tex, gl_TexCoord[1].st );
}
