varying vec3 L;
attribute vec3 tangent;
uniform vec3 light_obj_space_pos;
void main(void)
{
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

 vec3 bitangent = cross( gl_Normal, tangent  );
 
 vec3 v = light_obj_space_pos-gl_Vertex;
 L.x = dot( tangent,   v );
 L.y = dot( bitangent, v );
 L.z = dot( gl_Normal, v );
 #if 0
  L = normalize(L); // normalized in the fragment shader
 #else
 
 gl_FrontColor = gl_Color;
 gl_TexCoord[0] = gl_MultiTexCoord0;
}
