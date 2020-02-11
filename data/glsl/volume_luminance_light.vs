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

#version 330 core

// Simply passes the vertex frag_position and texture coordinate to the fragment shader.
// It also passes the vertex coord in object space to perform per-pixel lighting.

#pragma VL include /glsl/std/uniforms.glsl
#pragma VL include /glsl/std/vertex_attribs.glsl

out vec3 frag_position; // in object space
out vec4 tex_coord;

void main(void)
{
    gl_Position = vl_ModelViewProjectionMatrix * vl_VertexPosition;
    tex_coord = vl_VertexTexCoord0;
    frag_position = vl_VertexPosition.xyz;
}
// Have fun!
