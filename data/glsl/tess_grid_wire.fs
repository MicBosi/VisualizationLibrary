/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2020, Michele Bosi.                                            */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  This file is part of Visualization Library                                        */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Released under the OSI approved Simplified BSD License                            */
/*  http://www.opensource.org/licenses/bsd-license.php                                */
/*                                                                                    */
/**************************************************************************************/

#version 400 compatibility

uniform vec4 wire_color;

void main(void)
{
	gl_FragColor = wire_color;
}
