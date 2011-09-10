varying vec4 ECposition; // surface position in eye coordinates
varying vec4 ECballCenter; // ball center in eye coordinates

void main()
{
	const vec4 BallCenter = vec4(0.0, 0.0, 0.0, 1.0); // ball center in modeling coordinates
	
	ECposition = gl_ModelViewMatrix * gl_Vertex;
	ECballCenter = gl_ModelViewMatrix * BallCenter;
	gl_Position = ftransform();
}
