#version 330 core
layout (location = 0) in vec3 a_position;

layout(std140) uniform LightProj
{
	mat4 u_LightViewMatrix;
	mat4 u_LightProjectionMatrix;
};
uniform mat4 u_worldMatrix;

void main()
{
    gl_Position = u_LightProjectionMatrix * u_LightViewMatrix * u_worldMatrix * vec4(a_position, 1.0f);
}