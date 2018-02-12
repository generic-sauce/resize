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
out float clippingDepth;

void main()
{
	normalCameraspace = normalize(V * M * vec4(vertexNormal, 0));
	directionCameraspace = normalize(V * M * vec4(vertexPositionModelspace, 1));
	positionCameraspace = V * M * vec4(vertexPositionModelspace, 1);
	
	/*vec3 p = positionCameraspace.xyz + vec3(0, 0, 1) * 0.001f;
	vec3 e = vec3(clippingPositionCameraspace);
	vec3 a = p - e;
	vec3 n = clippingNormalCameraspace;
	vec3 u = normalize(cross(a, n));
	vec3 r = normalize(cross(n, u));
	float l = (p.x - e.x) / r.x;
	vec3 b = vec3(p.x - e.x, p.y - e.y, r.z * l);
	clippingDepth = l;*/

	if (abs(positionCameraspace.z - clippingPositionCameraspace.z) < 0.001f)
		clippingDepth = positionCameraspace.z;
	else {
		float distanceToCamera =
			(
				- (positionCameraspace.x - clippingPositionCameraspace.x) * clippingNormalCameraspace.x
				- (positionCameraspace.y - clippingPositionCameraspace.y) * clippingNormalCameraspace.y
			) / clippingNormalCameraspace.z
			+ clippingPositionCameraspace.z;
		clippingDepth = positionCameraspace.z - distanceToCamera;
	}

	gl_Position = MVP * vec4(vertexPositionModelspace, 1);
}
