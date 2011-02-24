/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi.                                            */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  This file is part of Visualization Library                                        */
/*  http://www.visualizationlibrary.com                                               */
/*                                                                                    */
/*  Released under the OSI approved Simplified BSD License                            */
/*  http://www.opensource.org/licenses/bsd-license.php                                */
/*                                                                                    */
/**************************************************************************************/

/* specular light texture */

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	vec3 N = normalize(gl_NormalMatrix * gl_Normal);
	vec3 V = gl_ModelViewMatrix * gl_Vertex;
	vec3 L = normalize(gl_LightSource[0].position - V.xyz);

	vec3 H = normalize(L + vec3(0.0,0.0,1.0));

	// compute diffuse equation
	float NdotL = dot(N,L);
	gl_FrontColor = gl_Color * vec4(max(0.0,NdotL));

	// encode specular texture
	float NdotH = NdotL > 0.0 ? max(0.0, dot(N,H)*8.0-7.0) : 0.0;
	gl_TexCoord[0] = vec4(NdotH,0.0,0.0,1.0);
}
