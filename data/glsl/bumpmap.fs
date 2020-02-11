/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2020, Michele Bosi.                                            */
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

in vec3 L;
in vec4 TexCoord;
in vec4 FrontColor;
uniform sampler2D sampler0;

void main(void)
{
	vec3 l = normalize(L);
	vec3 N = texture2D(sampler0, TexCoord.st, 0.0).rgb * 2.0 - 1.0;
	#if 0
	 vec3 n = normalize(N);
	#else
	 vec3 n = N;
	#endif

	// compute diffuse equation
	float NdotL = dot(n,l);
	vec4 diffuse = FrontColor * vec4(max(0.0,NdotL));

	gl_FragColor.rgb = diffuse.rgb;

	gl_FragColor.a = 1.0;
}
