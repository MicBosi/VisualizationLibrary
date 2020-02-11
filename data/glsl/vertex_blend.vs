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
#pragma VL include /glsl/std/vertex_attribs.glsl

// in vec3 vl_VertexNextPosition;
// in vec3 vl_VertexNextNormal;
uniform   float anim_t;

void main(void)
{
	gl_Position = vl_ModelViewProjectionMatrix * (vl_VertexPosition*(1.0-anim_t) + vl_VertexNextPosition*anim_t);
	vec3 N = normalize(vl_NormalMatrix * (vl_VertexNormal*(1.0-anim_t) + vl_VertexNextNormal*anim_t));

	vec3 V = (vl_ModelViewMatrix * vl_VertexPosition).xyz;
	vec3 L = normalize(gl_LightSource[0].position.xyz - V.xyz);
	vec3 H = normalize(L + vec3(0.0,0.0,1.0));

	// compute diffuse equation
	float NdotL = dot(N,L);
	vec4 diffuse = gl_Color * vec4(max(0.0,NdotL));

	float NdotH = max(0.0, dot(N,H));
	vec4 specular = vec4(0.0);
	const float specularExp = 128.0;
	if (NdotL > 0.0)
	  specular = vec4(pow(NdotH, specularExp));

	gl_FrontColor = diffuse + specular;
	gl_TexCoord[0] = vl_VertexTexCoord0;
}
