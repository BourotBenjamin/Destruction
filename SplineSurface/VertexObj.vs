#version 330

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoords;

uniform mat4 u_worldMatrix;

layout(std140) uniform ViewProj
{
	mat4 u_viewMatrix;
	mat4 u_projectionMatrix;
    vec3 viewPos;
    vec3 lightPos;
};

out Vertex
{
	vec3 FragPos;
	vec3 normal;
	vec2 texcoords;
	vec3 viewPos;
	vec3 lightPos;
} OUT;

void main(void)
{
	vec3 N = mat3(u_worldMatrix) * a_normal;
	OUT.FragPos = vec3(u_worldMatrix * vec4(a_position, 1.0f)); 
	OUT.normal = N;
	OUT.texcoords = vec2(a_texcoords.x, 1.0 - a_texcoords.y);
	OUT.viewPos = vec3(u_projectionMatrix * u_viewMatrix * vec4(viewPos, 1.0f));
	OUT.lightPos = vec3(u_projectionMatrix * u_viewMatrix *vec4(lightPos, 1.0f));
	gl_Position = u_projectionMatrix * u_viewMatrix * u_worldMatrix * vec4(a_position, 1.0f);

}