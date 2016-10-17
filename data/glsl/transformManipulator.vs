/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi, Fabien Mathieu.							  */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  This file is part of Visualization Library                                        */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Released under the OSI approved Simplified BSD License                            */
/*  http://www.opensource.org/licenses/bsd-license.php                                */
/*                                                                                    */
/**************************************************************************************/

#version 330 core

uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ManipulatorPositionMatrix;
uniform mat4 ProjectionMatrix;

in vec4 VertexPosition;


void main(void)
{
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * ManipulatorPositionMatrix * VertexPosition;
}
