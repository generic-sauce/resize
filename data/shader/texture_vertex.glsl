#version 330 core

layout (location = 1) in vec3 vertexPositionModelspace;
layout (location = 2) in vec3 vertexUv;

out vec2 uv;

void main()
{
	uv = vertexUv.xy;

	gl_Position = vec4(vertexPositionModelspace, 1);
}
