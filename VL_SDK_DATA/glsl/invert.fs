void main(void)
{
	gl_FragColor.rgb = 1.0 - clamp(gl_Color + gl_SecondaryColor, 0.0, 1.0);
	gl_FragColor.a = 1.0;
}
