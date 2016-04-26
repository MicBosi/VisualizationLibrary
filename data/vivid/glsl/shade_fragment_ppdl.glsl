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
varying vec3 V;

vec4 ShadeFragment()
{
  vec3 l = normalize(gl_LightSource[0].position.xyz - V);
  vec3 e = normalize(vec3(0,0,0) - V); // vec3(0.0,0.0,1.0) for GL_LIGHT_MODEL_LOCAL_VIEWER = FALSE
  vec3 n = normalize(N);
  vec3 H = normalize(l + e);

  // compute diffuse equation
  float NdotL = dot(n,l);
  // dual side lighting
  if (NdotL < 0) {
    NdotL = abs(NdotL);
    n = n * -1;
  }

  vec3 diffuse = gl_FrontMaterial.diffuse.rgb * gl_LightSource[0].diffuse.rgb;
  diffuse = diffuse * vec3(max(0.0,NdotL));

  float NdotH = max(0.0, dot(n,H));
  vec3 specular = vec3(0.0);
  if (NdotL > 0.0) {
    specular = gl_FrontMaterial.specular.rgb * gl_LightSource[0].specular.rgb * pow(NdotH, gl_FrontMaterial.shininess);
  }

  vec3 ambient  = gl_FrontMaterial.ambient.rgb * gl_LightSource[0].ambient.rgb + gl_FrontMaterial.ambient.rgb * gl_LightModel.ambient.rgb;
  vec3 emission = gl_FrontMaterial.emission.rgb;

  vec3 color = ambient + emission + diffuse + specular;

  // NOTE: we take the alpha from the front material's diffuse color
  float alpha = gl_FrontMaterial.diffuse.a;

  return vec4(color, alpha);
}
