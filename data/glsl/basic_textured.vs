void main(void)
{
	gl_Position    = ftransform();
	gl_FrontColor  = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
