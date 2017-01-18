/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2017, Michele Bosi.                                            */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  This file is part of Visualization Library                                        */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Released under the OSI approved Simplified BSD License                            */
/*  http://www.opensource.org/licenses/bsd-license.php                                */
/*                                                                                    */
/**************************************************************************************/

#version 150 compatibility

// Simply passes the vertex frag_position and texture coordinate to the fragment shader.
// It also passes the vertex coord in object space to perform per-pixel lighting.

out vec3 frag_position; // in object space

void main(void)
{
    gl_Position = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
    frag_position = gl_Vertex.xyz;
}
// Have fun!
