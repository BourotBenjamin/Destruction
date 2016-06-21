#version 330

 // Direction VERS la source lumineuse exprimee dans le repere WORLD
const vec3 L = vec3(0.0, 0.0, 1.0);

uniform sampler2D u_sampler;
uniform sampler2D u_specularMap;

uniform vec3 u_ambiant;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess;

in Vertex
{
	vec3 FragPos;
	vec3 normal;
	vec2 texcoords;
	vec3 viewPos;
	vec3 lightPos;
} IN;

out vec4 Fragment;

void main(void)
{
    vec4 texColor = texture(u_sampler, IN.texcoords);

    vec3 lightAmbiant = vec3(0.3);
    vec3 lightDiffuse = vec3(0.5);
    vec3 lightSpecular = vec3(0.7);

	// Ambient
    vec3 ambient = lightAmbiant * texColor.rgb;
  	
    // Diffuse 
    vec3 norm = normalize(IN.normal);
    vec3 lightDir = normalize(IN.lightPos - IN.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * (diff * texColor.rgb);
    
    // Specular
    vec3 viewDir = normalize(IN.viewPos - IN.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = lightSpecular * spec * vec3(texture(u_specularMap, IN.texcoords)); 
        
    vec3 result = ambient + diffuse + specular;

    Fragment = vec4(result, 1.0f);
}