//--------------------------------------------------------------------------------------
// Order Independent Transparency with Average Color
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
// Improved by Michele Bosi for VisualizationLibrary.org
//--------------------------------------------------------------------------------------

#version 130

uniform sampler2DRect ColorTex0;
uniform sampler2DRect ColorTex1;
uniform vec3 BackgroundColor;

void main(void)
{
  vec4 SumColor = texture2DRect(ColorTex0, gl_FragCoord.xy);
  float n = texture2DRect(ColorTex1, gl_FragCoord.xy).r;

  if (n == 0.0) {
    gl_FragColor.rgb = BackgroundColor;
    return;
  }

  vec3 AvgColor = SumColor.rgb / SumColor.a;
  float AvgAlpha = SumColor.a / n;

  float T = pow(1.0-AvgAlpha, n);
  gl_FragColor.rgb = AvgColor * (1 - T) + BackgroundColor * T;
  gl_FragColor.a = 1.0;
}
