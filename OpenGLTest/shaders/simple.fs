#version 330 core

uniform mat4 uMV;

in vec3 vMVPosition;
in vec3 vNormal;

out vec3 color;

void main()
{
	vec3 lightSource = vec3(1, 3, -2);
	vec3 mvLightSource = (uMV * vec4(lightSource, 1)).xyz;

	vec3 toLight = mvLightSource - vMVPosition;
	float colorValue = max(0, dot(normalize(toLight), normalize(vNormal)));

	color = vec3(1, 0, 0) * colorValue;
}