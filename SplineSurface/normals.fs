#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec4 FragPos;
    vec3 Normal;
    vec3 lightPos;
    vec3 viewPos;
    vec3 worldPos;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D u_sampler;
uniform sampler2D u_specularMap;
uniform sampler2D u_shadowMap;
uniform sampler2D u_normalMap;

uniform vec3 u_ambiant;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess;
uniform int u_isSpecular;

uniform bool normalMapping;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normalBumped)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(normalBumped);
    vec3 lightDir = normalize(fs_in.lightPos - fs_in.FragPos.xyz);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    //vec4 projCoords = textureProj(u_shadowMap, fragPosLightSpace, bias);

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    
    // Check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    //float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    vec3 lightAmbiant = vec3(0.3);
    vec3 lightDiffuse = vec3(1.0);
    vec3 lightSpecular = vec3(0.3);
    float LightPower = 1.0;

    vec3 ambiantColor = lightAmbiant * lightDiffuse;
    vec3 diffuseColor = vec3(0,0,0);
    vec3 specularColor = vec3(0,0,0);//u_specular * );

    vec3 normal = fs_in.Normal;
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal);

    float distance = length( fs_in.lightPos - fs_in.FragPos.xyz );
    // Ambient

    // Diffuse
    vec3 lightDir = normalize(fs_in.lightPos - fs_in.FragPos.xyz);
    float diff = dot(normal, lightDir);

    if(diff > 0)
    {
        diffuseColor = lightDiffuse * diff * LightPower;
    }

    vec4 texColor = vec4(1.0, 0.5, 0.5, 1.0);
    FragColor = vec4(ambiantColor + (1.0 - shadow) * (diffuseColor + specularColor), 1.0) * texColor;
}

