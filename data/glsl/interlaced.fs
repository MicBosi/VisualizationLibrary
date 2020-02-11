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

void main(void)
{
	gl_FragColor = gl_FrontMaterial.diffuse;
	if ( int(gl_FragCoord.y) - int(gl_FragCoord.y)/2*2 == 1)
	{
		gl_FragColor.rgb *= 0.5;
	}
}
