varying vec3 N;
varying vec3 L;
void main(void)
{
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 vec3 V = (gl_ModelViewMatrix * gl_Vertex).xyz;
 L = normalize(gl_LightSource[0].position.xyz - V.xyz);
 N = normalize(gl_NormalMatrix * gl_Normal);
 gl_FrontColor = gl_Color;
}
