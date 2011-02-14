uniform sampler3D sampler0;
uniform float alpha_bias;
void main(void)
{
	vec4 color = texture3D(sampler0, gl_TexCoord[0].xyz );
	if (color.a >= alpha_bias) 
	{
		// color.a *= alpha_bias;
		gl_FragColor = color;
	}
	else
		discard;
}
