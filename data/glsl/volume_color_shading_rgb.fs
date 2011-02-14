uniform sampler3D sampler0;
uniform float alpha_bias;
void main(void)
{
	vec3 col = texture3D(sampler0, gl_TexCoord[0].xyz ).rgb;
	gl_FragColor.rgb = col;
	float gray = dot( col, vec3(0.299,0.587,0.114) );
	gl_FragColor.a = gray * alpha_bias;
}
