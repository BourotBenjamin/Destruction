#version 330 core
layout (location = 0) in vec3 a_position;

uniform mat4 u_worldMatrix;

layout(std140) uniform ViewProj
{
	mat4 u_viewMatrix;
	mat4 u_projectionMatrix;
    vec3 u_viewPos;
};

layout(std140) uniform LightProj
{
    mat4 u_lightViewMatrix;
    mat4 u_lightProjectionMatrix;
    mat4 u_lightCoordrectifier;
    vec3 u_lightPos;
};

void main()
{
    gl_Position = u_projectionMatrix * u_viewMatrix * u_worldMatrix * vec4(a_position, 1.0f);
}