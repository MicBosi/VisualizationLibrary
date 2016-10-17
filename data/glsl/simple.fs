#version 330 core

flat in vec4 FrontColor;

out vec4 FragColor;

void main()
{
	FragColor = FrontColor;
}