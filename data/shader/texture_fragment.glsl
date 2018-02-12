#version 330 core

uniform sampler2D tex;

in vec2 uv;

void main()
{
	vec4 p = texture2D(tex, uv).rgba;
	gl_FragColor = vec4(p.rgb * 1000.f, 1);
}
