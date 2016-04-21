//--------------------------------------------------------------------------------------
// Order Independent Transparency with Average Color
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
// Improved by Michele Bosi for VisualizationLibrary.org
//--------------------------------------------------------------------------------------

#version 120

vec4 ShadeFragment();

void main(void)
{
  vec4 color = ShadeFragment();
  gl_FragData[0] = vec4(color.rgb * color.a, color.a);
  gl_FragData[1] = vec4(1.0);
}
