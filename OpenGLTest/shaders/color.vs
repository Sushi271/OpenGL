#version 330 core

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uNormalMatrix;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;

out vec3 vMVPosition;
out vec3 vNormal;
out vec3 vColor;

void main()
{
	vMVPosition = (uMV * (vec4(aPosition, 1))).xyz;
    gl_Position = uMVP * vec4(aPosition, 1);

	vNormal = (uNormalMatrix * vec4(aNormal, 1)).xyz;
	vColor = aColor;
}

