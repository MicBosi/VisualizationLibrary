//--------------------------------------------------------------------------------------
// Order Independent Transparency with Dual Depth Peeling
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

void main(void)
{
  gl_Position = ftransform();
  V = (gl_ModelViewMatrix * gl_Vertex).xyz;
  N = normalize(gl_NormalMatrix * gl_Normal);
  // gl_FrontColor = gl_FrontMaterial.diffuse;
}
