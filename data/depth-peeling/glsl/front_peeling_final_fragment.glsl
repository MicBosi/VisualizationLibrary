//--------------------------------------------------------------------------------------
// Order Independent Transparency with Depth Peeling
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#version 130

uniform sampler2DRect ColorTex;
uniform vec3 BackgroundColor;

void main(void)
{
	vec4 frontColor = texture2DRect(ColorTex, gl_FragCoord.xy);
	gl_FragColor.rgb = frontColor.rgb + BackgroundColor.rgb * frontColor.a;
}
