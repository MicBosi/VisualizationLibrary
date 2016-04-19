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

void main(void)
{
  gl_FragColor.xy = vec2(-gl_FragCoord.z, gl_FragCoord.z);
}
