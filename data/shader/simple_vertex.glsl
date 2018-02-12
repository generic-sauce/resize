#version 330 core

layout (location = 1) in vec3 vertexPositionModelspace;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(vertexPositionModelspace, 1);
}
