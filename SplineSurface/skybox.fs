#version 330

in vec3 v_texCoords;

out vec4 Fragment;

uniform samplerCube u_cubeMap;

void main(void)
{
	Fragment = texture(u_cubeMap, v_texCoords);
}
