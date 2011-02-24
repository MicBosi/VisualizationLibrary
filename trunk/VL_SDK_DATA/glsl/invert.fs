/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi.                                            */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  This file is part of Visualization Library                                        */
/*  http://www.visualizationlibrary.com                                               */
/*                                                                                    */
/*  Released under the OSI approved Simplified BSD License                            */
/*  http://www.opensource.org/licenses/bsd-license.php                                */
/*                                                                                    */
/**************************************************************************************/

void main(void)
{
	gl_FragColor.rgb = 1.0 - clamp(gl_Color + gl_SecondaryColor, 0.0, 1.0);
	gl_FragColor.a = 1.0;
}
