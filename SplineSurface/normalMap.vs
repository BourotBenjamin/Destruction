#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoords;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;

out VS_OUT {
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    vec3 lightPos;
    vec3 viewPos;
    vec3 worldPos;
    mat3 TBN;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
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
    vs_out.TexCoords = vec2(a_texcoords.x, 1.0 - a_texcoords.y);
    mat3 normalMatrix = transpose(inverse(mat3(u_worldMatrix)));
    vs_out.Normal = normalize(normalMatrix * a_normal);
    //vs_out.Normal = (u_worldMatrix * vec4(a_normal, 1.0f)).xyz;
    vs_out.lightPos = u_lightPos;
    vs_out.viewPos = u_viewPos;
    vs_out.worldPos =  (u_worldMatrix * vec4(a_position, 1.0f)).xyz;
    vec3 T = normalize(normalMatrix * a_tangent);
    vec3 B = normalize(normalMatrix * a_bitangent);
    vec3 N = normalize(normalMatrix * a_normal);
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TBN = TBN;
    
    vs_out.TangentLightPos = TBN * u_lightPos;//vec3(u_projectionMatrix * u_viewMatrix * vec4(lightPos, 1.0f));;
    vs_out.TangentViewPos  = TBN * u_viewPos;//vec3(u_projectionMatrix * u_viewMatrix * vec4(viewPos, 1.0f));;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos.xyz;
    
    vs_out.Tangent = (u_worldMatrix * vec4(T, 1.0f)).xyz;
    vs_out.Bitangent = B;
    vs_out.FragPosLightSpace = u_lightCoordrectifier * u_lightProjectionMatrix * u_lightViewMatrix * vs_out.FragPos;
}