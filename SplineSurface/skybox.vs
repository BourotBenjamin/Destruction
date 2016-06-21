#version 330

layout (location = 0) in vec3 a_position;
out vec3 v_texCoords;

layout(std140) uniform ViewProj
{
	mat4 u_viewMatrix;
	mat4 u_projectionMatrix;
};

void main(void)
{
	// il est necessaire de specifier position.w=0.0 car l'on souhaite que la skybox reste dans l'axe de la camera
	// la skybox va donc subir les rotations de la camera mais va rester alignee avec celle-ci.
	// On n'a donc aucun besoin d'avoir une matrice "local to world"

	vec4 pos = u_projectionMatrix * u_viewMatrix * vec4(a_position, 0.0);
	
	// une skybox est censee definir un environnement distant or notre cube lui est unitaire (-1, +1) dans les 3 axes.
	// la solution est de forcer le Z a 1.0 en mettant la meme valeur pour z et w.
	// Ainsi lors de la division perspective qui produit (x/w, y/w, z/w) on aura bien un fragment positionne sur le plan FAR.

	gl_Position = pos.xyww; // cela fonctionne aussi avec pos.xyzz
	
	// la magie opere aussi dans le rasterizer: on utilise les 8 sommets du cube comme representant des directions.
	// le rasterizer va interpoler ces directions et va donc ainsi sampler la cubemap comme si il y'avait un lancer de rayon
	// de l'origine du cube au texel concernee.
	v_texCoords = a_position;
}