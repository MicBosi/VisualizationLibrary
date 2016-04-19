//--------------------------------------------------------------------------------------
// Order Independent Transparency with Weighted Sums
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
// Improved by Michele Bosi for VisualizationLibrary.org
//--------------------------------------------------------------------------------------

#version 130

vec3 ShadeVertex();

void main(void)
{
  gl_Position = ftransform();
  gl_TexCoord[0].xyz = ShadeVertex();
}
