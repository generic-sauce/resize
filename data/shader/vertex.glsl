#version 330 core

layout (location = 1) in vec3 vertexPositionModelspace;
layout (location = 2) in vec3 vertexNormal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 MVP;
uniform vec3 clippingPositionCameraspace;
uniform vec3 clippingNormalCameraspace;

out vec4 normalCameraspace;
out vec4 directionCameraspace;
out vec4 positionCameraspace;
out float portalDistance;

void main()
{
	normalCameraspace = normalize(V * M * vec4(vertexNormal, 0));
	directionCameraspace = normalize(V * M * vec4(vertexPositionModelspace, 1));
	positionCameraspace = V * M * vec4(vertexPositionModelspace, 1);

	if (clippingPositionCameraspace == vec3(0, 0, 0) && clippingNormalCameraspace == vec3(0, 0, 0))
		portalDistance = 0;
	else if (abs(positionCameraspace.z - clippingPositionCameraspace.z) < 0.001f)
		portalDistance = positionCameraspace.z;
	else {
		float distanceToCamera =
			(
				- (positionCameraspace.x - clippingPositionCameraspace.x) * clippingNormalCameraspace.x
				- (positionCameraspace.y - clippingPositionCameraspace.y) * clippingNormalCameraspace.y
			) / clippingNormalCameraspace.z + clippingPositionCameraspace.z;
		portalDistance = positionCameraspace.z - distanceToCamera;
	}

	gl_Position = MVP * vec4(vertexPositionModelspace, 1);
}
