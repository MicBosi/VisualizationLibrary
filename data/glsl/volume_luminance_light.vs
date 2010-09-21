// This shader maps the volume value to the transfer function plus computes the
// gradient and lighting on the fly. This shader is to be used with IF_LUMINANCE
// image volumes.

varying vec3 position; // in object space

void main(void)
{
  position = gl_Vertex.xyz;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}
