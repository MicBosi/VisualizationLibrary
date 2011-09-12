/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi.                                            */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  This file is part of Visualization Library                                        */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Released under the OSI approved Simplified BSD License                            */
/*  http://www.opensource.org/licenses/bsd-license.php                                */
/*                                                                                    */
/**************************************************************************************/

// adaptation from the "Orange Book" (http://www.3dshaders.com)

varying vec3 MCposition;
varying float LightIntensity;

#define Integral(x, p, notp) ((floor(x)*(p)) + max(fract(x)-(notp), 0.0))

void main()
{
	const vec3 BrickColor = vec3(0.7, 0.2, 0.0);
	const vec3 MortarColor = vec3(0.5, 0.5, 0.5);
	const vec2 BrickSize = vec2(0.2);
	const vec2 BrickPct = vec2(0.85);
	const vec2 MortarPct = vec2(0.15);

	vec2 position, fw, useBrick;
	vec3 color;
	
	position = MCposition.xy / BrickSize;
	
	if (fract(position.y * 0.5) > 0.5)
		position.x += 0.5;
		
	// fwidth - return the sum of the absolute derivatives in x and y
	fw = fwidth(position);
	
	useBrick = (Integral(position + fw, BrickPct, MortarPct) - 
	            Integral(position, BrickPct, MortarPct)) / fw;
	
	color = mix(MortarColor, BrickColor, useBrick.x * useBrick.y);
	color *= LightIntensity;
	gl_FragColor = vec4(color, 1.0);
}
