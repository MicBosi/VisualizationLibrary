#version 330 core

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

in vec4 VertexPosition;
in vec4 VertexColor;

flat out vec4 FrontColor;

void main()
{
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * VertexPosition;
	FrontColor = VertexColor;
}