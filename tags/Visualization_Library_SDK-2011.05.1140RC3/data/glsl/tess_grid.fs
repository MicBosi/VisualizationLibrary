/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi.                                            */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  This file is part of Visualization Library                                        */
/*  http://www.visualizationlibrary.com                                               */
/*                                                                                    */
/*  Released under the OSI approved Simplified BSD License                            */
/*  http://www.opensource.org/licenses/bsd-license.php                                */
/*                                                                                    */
/**************************************************************************************/

#version 400 compatibility

uniform sampler2D tex_diffuse;
in vec2 tex_coord;

void main(void)
{
	vec3 diffuse = texture(tex_diffuse, tex_coord).rgb;
	gl_FragColor = vec4( diffuse, 1.0);
}
