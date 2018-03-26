#version 330 core

in vec4 normalCameraspace;
in vec4 directionCameraspace;
in vec4 positionCameraspace;
in float clippingDepth;

void main()
{
	//if (-clippingDepth < 0.f)
	//	discard;

	vec4 cameraDirection = vec4(0, 0, -1, 0);

	float angleFactor = clamp(dot(cameraDirection, -normalCameraspace), 0, 1);
	float directionFactor = clamp(dot(cameraDirection, directionCameraspace), 0, 1);

	gl_FragColor = vec4(0, 0, 0, 1)
		+ vec4(1, 1, 1, 1) * 0.6
		+ vec4(1, 1, 1, 1) * 0.4 * angleFactor
		- vec4(1, 1, 1, 1) * 0.5 *  (-1 / ((length(positionCameraspace) / 10) + 1) + 1);
		//- vec4(1, 1, 1, 1) * 0.6 * directionFactor;
	
	/*if (int(floor(gl_FragCoord.x)) % 2 == 0)
		gl_FragColor = vec4(1, 1, 1, 1) * .5f;
	else
		gl_FragColor = vec4(clippingDepth, -clippingDepth, abs(clippingDepth) < 0.0001f, 1);*/

}
