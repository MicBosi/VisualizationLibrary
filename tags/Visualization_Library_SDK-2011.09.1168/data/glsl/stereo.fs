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

uniform sampler2D left_channel;
uniform sampler2D right_channel;
varying vec4 V;

void main(void)
{
	vec4 left_color = texture2D(left_channel, V.st);
	vec4 right_color = texture2D(right_channel, V.st);
	gl_FragColor = vec4(left_color.r, right_color.g, right_color.b, 1.0);
}
