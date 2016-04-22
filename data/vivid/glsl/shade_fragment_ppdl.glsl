//--------------------------------------------------------------------------------------
// Order Independent Transparency Fragment Shader
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
// Improved by Michele Bosi for VisualizationLibrary.org
//--------------------------------------------------------------------------------------

#version 120

varying vec3 N;
varying vec3 L;

vec4 ShadeFragment()
{
  vec3 l = normalize(L);
  vec3 n = normalize(N);
  vec3 H = normalize(l + vec3(0.0,0.0,1.0));

  // compute diffuse equation
  float NdotL = dot(n,l);
  // dual side lighting
  if (NdotL < 0) {
    NdotL = abs(NdotL);
    n = n * -1;
  }
  vec4 diffuse = gl_FrontMaterial.diffuse;
  diffuse.rgb = diffuse.rgb * vec3(max(0.0,NdotL));

  float NdotH = max(0.0, dot(n,H));
  vec4 specular = vec4(0.0);
  const float specularExp = 128.0;
  if (NdotL > 0.0)
    specular = vec4(pow(NdotH, specularExp));

  // return vec4((diffuse + specular).rgb, 0.5);
  return vec4((diffuse + specular).rgb, diffuse.a);
}
