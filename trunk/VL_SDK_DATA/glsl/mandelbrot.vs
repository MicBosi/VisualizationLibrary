varying vec3 Position;

void main(void)
{
	Position = vec3(gl_MultiTexCoord0 - 0.5) * 0.5;
	gl_Position = ftransform();
}
