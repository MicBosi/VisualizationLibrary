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

varying vec3 N;
varying vec3 L;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	vec3 V = (gl_ModelViewMatrix * gl_Vertex).xyz;
	L = normalize(gl_LightSource[0].position.xyz - V);
	N = normalize(gl_NormalMatrix * gl_Normal);
	gl_FrontColor = gl_Color;
}
