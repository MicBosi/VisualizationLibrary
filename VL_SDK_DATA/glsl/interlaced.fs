void main(void)
{
	gl_FragColor = gl_FrontMaterial.diffuse;
	if ( int(gl_FragCoord.y) - int(gl_FragCoord.y)/2*2 == 1)
	{
		gl_FragColor.rgb *= 0.5;
	}
}
