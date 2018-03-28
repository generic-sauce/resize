#version 330 core

uniform vec2 viewportSize;
uniform sampler2D portalDepthTex;

in vec4 normalCameraspace;
in vec4 directionCameraspace;
in vec4 positionCameraspace;
in float portalDistance;

void main()
{
	if (portalDistance > 0.f)
		discard;
	if (gl_FragCoord.z > texture(portalDepthTex, gl_FragCoord.xy / viewportSize).r)
		discard;

	vec4 cameraDirection = vec4(0, 0, -1, 0);

	float angleFactor = clamp(dot(cameraDirection, -normalCameraspace), 0, 1);
	float directionFactor = clamp(dot(cameraDirection, directionCameraspace), 0, 1);

	gl_FragColor = vec4(0, 0, 0, 1)
		+ vec4(1, 1, 1, 1) * 0.6
		+ vec4(1, 1, 1, 1) * 0.4 * angleFactor
		- vec4(1, 1, 1, 1) * 0.5 *  (-1 / ((length(positionCameraspace) / 10) + 1) + 1);
	
	gl_FragColor *= 2.f * vec4(
		portalDistance > 0.f ? 1.f : 0.f,
		portalDistance < 0.f ? 1.f : 0.f,
		portalDistance == 0.f,
		1);

	/*if (gl_FragCoord.z > texture(portalDepthTex, gl_FragCoord.xy / viewportSize).r) {
		gl_FragColor = vec4(1, 1, 0, 1);
	}*/
}
