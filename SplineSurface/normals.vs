#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

out VS_OUT {
    vec4 FragPos;
    vec3 Normal;
    vec3 lightPos;
    vec3 viewPos;
    vec3 worldPos;
    vec4 FragPosLightSpace;
} vs_out;

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
    vs_out.FragPos = u_worldMatrix * vec4(a_position, 1.0f);   
    mat3 normalMatrix = transpose(inverse(mat3(u_worldMatrix)));
    vs_out.Normal = normalize(normalMatrix * a_normal);
    vs_out.lightPos = u_lightPos;
    vs_out.viewPos = u_viewPos;
    vs_out.worldPos =  (u_worldMatrix * vec4(a_position, 1.0f)).xyz;
    vs_out.FragPosLightSpace = u_lightCoordrectifier * u_lightProjectionMatrix * u_lightViewMatrix * vs_out.FragPos;
}