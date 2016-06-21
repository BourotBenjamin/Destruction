#version 400                                                                        
                                                                                    
layout(points) in;                                                                  
layout(triangle_strip) out;                                                         
layout(max_vertices = 4) out;     

uniform mat4 u_worldMatrix;                                                  
                                                                                    
layout(std140) uniform ViewProj
{
    mat4 u_viewMatrix;
    mat4 u_projectionMatrix;
    vec3 u_viewPos;
};

//uniform float u_billboardSize;                                                       

out GS_OUT {                                                                       
    vec2 TexCoord;  
}gs_out;                                                                  
                                                                                    
void main()                                                                         
{          
    float u_billboardSize = 1.0f;                                                                         
    vec3 Pos = gl_in[0].gl_Position.xyz;                                            
    vec3 toCamera = normalize(u_viewPos - Pos);                                    
    vec3 up = vec3(0.0, 1.0, 0.0);                                                  
    vec3 right = cross(toCamera, up) * u_billboardSize;                              
                                                                                    
    Pos -= right;                                                                   
    gl_Position = u_projectionMatrix * u_viewMatrix * u_worldMatrix * vec4(Pos, 1.0);                                             
    gs_out.TexCoord = vec2(0.0, 0.0);                                                      
    EmitVertex();                                                                   
                                                                                    
    Pos.y += u_billboardSize;                                                        
    gl_Position = u_projectionMatrix * u_viewMatrix *  u_worldMatrix * vec4(Pos, 1.0);                                             
    gs_out.TexCoord = vec2(0.0, 1.0);                                                      
    EmitVertex();                                                                   
                                                                                    
    Pos.y -= u_billboardSize;                                                        
    Pos += right;                                                                   
    gl_Position = u_projectionMatrix * u_viewMatrix *  u_worldMatrix * vec4(Pos, 1.0);                                             
    gs_out.TexCoord = vec2(1.0, 0.0);                                                      
    EmitVertex();                                                                   
                                                                                    
    Pos.y += u_billboardSize;                                                        
    gl_Position = u_projectionMatrix * u_viewMatrix *  u_worldMatrix * vec4(Pos, 1.0);                                             
    gs_out.TexCoord = vec2(1.0, 1.0);                                                      
    EmitVertex();                                                                   
                                                                                    
    EndPrimitive();                                                                 
}                                                                                   
