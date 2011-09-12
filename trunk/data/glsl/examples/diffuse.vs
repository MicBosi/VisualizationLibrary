/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi.                                            */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  This file is part of Visualization Library                                        */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Released under the OSI approved Simplified BSD License                            */
/*  http://www.opensource.org/licenses/bsd-license.php                                */
/*                                                                                    */
/**************************************************************************************/

// main function
void main(void)
{
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 vec3 N = normalize(gl_NormalMatrix * gl_Normal);
 vec3 V = (gl_ModelViewMatrix * gl_Vertex).xyz;
 vec3 L = normalize(gl_LightSource[0].position.xyz - V.xyz);
 // compute diffuse equation
 float NdotL = dot(N,L);
 gl_FrontColor = gl_Color * vec4(max(0.0,NdotL));
}
