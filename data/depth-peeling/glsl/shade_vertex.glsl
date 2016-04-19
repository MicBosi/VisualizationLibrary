//--------------------------------------------------------------------------------------
// Order Independent Transparency Vertex Shader
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
// Improved by Michele Bosi for VisualizationLibrary.org
//--------------------------------------------------------------------------------------

#version 130

vec3 ShadeVertex()
{
  float diffuse = abs(normalize(gl_NormalMatrix * gl_Normal).z);
  return vec3(gl_Vertex.xy, diffuse);
}
