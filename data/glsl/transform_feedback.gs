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

#version 330 core

layout(triangles) in;
layout(points, max_vertices = 1) out;

flat out int normale;

void main(void)
{
	/*vec3 a = normalize((gl_in[1].gl_Position - gl_in[0].gl_Position).xyz);
	vec3 b = normalize((gl_in[2].gl_Position - gl_in[0].gl_Position).xyz);
	normale = normalize(cross(a, b));*/
	normale = gl_PrimitiveIDIn;
	
	//for(int i = 0; i < 3; ++i)
	//{
		gl_Position = gl_in[0].gl_Position;
		EmitVertex();
	//}
	
	EndPrimitive();
}
