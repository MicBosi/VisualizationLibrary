void main(void)
{
 gl_Position   = gl_ModelViewProjectionMatrix * gl_Vertex;
 gl_FrontColor = gl_Color;
}
