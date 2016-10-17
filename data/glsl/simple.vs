#version 330 core

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec4 Color;

in vec4 VertexPosition;

flat out vec4 FrontColor;

void main()
{
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * VertexPosition;
	FrontColor = Color;
}