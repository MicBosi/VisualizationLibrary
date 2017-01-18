/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2017, Michele Bosi.                                            */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  This file is part of Visualization Library                                        */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Released under the OSI approved Simplified BSD License                            */
/*  http://www.opensource.org/licenses/bsd-license.php                                */
/*                                                                                    */
/**************************************************************************************/

#version 150 compatibility

#pragma VL include /glsl/std/uniforms.glsl
#pragma VL include /glsl/std/vertex_attribs.glsl

in vec3 VertexTangent;
uniform vec3 light_obj_space_pos;
out vec3 L;
out vec4 TexCoord;
out vec4 FrontColor;
void main(void)
{
  gl_Position = vl_ModelViewProjectionMatrix * vl_VertexPosition;

  vec3 bitangent = cross( vl_VertexNormal, VertexTangent  );

  vec3 v = light_obj_space_pos.xyz - vl_VertexPosition.xyz;
  L.x = dot( VertexTangent,   v );
  L.y = dot( bitangent, v );
  L.z = dot( vl_VertexNormal, v );
  #if 0
  L = normalize(L); // normalized in the fragment shader
  #endif

  FrontColor = gl_Color;
  TexCoord = vl_VertexTexCoord0;
}
