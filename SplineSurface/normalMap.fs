#version 330 core
out vec4 FragColor;

in VS_OUT {
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

vec3 CalcBumpedNormal()
{
    vec3 NNormal = normalize(fs_in.Normal);
    vec3 NTangent = normalize(fs_in.Tangent);
    NTangent = normalize(NTangent - dot(NTangent, NNormal) * NNormal);
    vec3 NBitangent = cross(NTangent, NNormal);//fs_in.Bitangent;//cross(NTangent, NNormal);*/
    vec3 BumpMapNormal = texture(u_normalMap, fs_in.TexCoords).rgb;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(NTangent, NBitangent, NNormal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normalBumped)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(normalBumped);
    //vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.FragPos.xyz);
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
    vec3 diffuseColor = vec3(0,0,0);//texture(u_sampler, fs_in.TexCoords).rgb;
    vec3 specularColor = vec3(0,0,0);//u_specular * );

    vec3 normal = CalcBumpedNormal();
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal);

    float distance = length( fs_in.lightPos - fs_in.FragPos.xyz );
    // Ambient

    // Diffuse
    vec3 lightDir = normalize(fs_in.lightPos - fs_in.TangentFragPos);
    float diff = dot(normal, lightDir);

    if(diff > 0)
    {
        diffuseColor = lightDiffuse * diff * LightPower;
        vec3 VertexToEye = normalize(fs_in.viewPos - fs_in.worldPos);
        vec3 LightReflect = normalize(reflect(fs_in.TangentLightPos, normal));//lightDir, normal)); 
        //Specular
        float SpecularFactor = dot(VertexToEye, LightReflect);
        if (SpecularFactor > 0) {                                                           
            SpecularFactor = pow(SpecularFactor, u_shininess);                               
            specularColor = vec3(lightDiffuse * SpecularFactor);
            if(u_isSpecular == 1)
                specularColor *= vec3(texture(u_specularMap, fs_in.TexCoords));
            else
                specularColor *= u_specular;
        }  
    }

    vec4 texColor = texture(u_sampler, fs_in.TexCoords);
    FragColor = vec4(ambiantColor + (1.0 - shadow) * (diffuseColor + specularColor), 1.0) * texColor;

    //vec 3 lightDir(0.f, 15.f, -25.f);
    //vec4 res = vec4(lightDir,1.0) - fs_in.FragPos;

    //FragColor = vec4(1, 1, 1, 1);// * res;

}

