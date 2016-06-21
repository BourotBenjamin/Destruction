#version 400                                                                        
                                                                                    
layout(points) in;                                                                  
layout(points) out;                                                                 
layout(max_vertices = 30) out;                                                      
                                                                                    
// in VS_IN {                                                                       
//     vec3 Position;                                                                 
//     vec3 Velocity;                                                                 
//     float Age;    
// }gs_in[];  

                                                                     
in vec3 in_Position0[];                                                                 
in vec3 in_Velocity0[];                                                                 
in float in_Age0[];    
                                                                    

/*out GS_OUT {                                                                       
    vec3 Position;                                                                 
    vec3 Velocity;                                                                 
    float Age;    
}gs_out;*/ 

                                                                     
out vec3 out_Position;                                                                 
out vec3 out_Velocity;                                                                 
out float out_Age;    

                                                                                                                                              
uniform float u_deltaTime;                                                     
uniform float u_time;                                                                
uniform sampler1D u_random;                                             
                                                                                    
#define PARTICLE_TYPE_LAUNCHER 0.0f                                                 
#define PARTICLE_TYPE_SHELL 1.0f                                                    
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f                                          
                                                                                    
vec3 GetRandomDir(float TexCoord)                                                   
{                                                                                   
     vec3 Dir = texture(u_random, TexCoord).xyz;                              
     Dir -= vec3(0.5, 0.5, 0.5);                                                    
     return Dir;                                                                    
}                                                                                   
                                                                                    
void main()                                                                         
{                                                                                   
    // float Age = in_Age[0] + u_deltaTime;                                         
                                                                                                                                                             
     float DeltaTimeSecs = u_deltaTime ;//s/ 1000.0f;                           
    // float t1 = in_Age[0] / 1000.0;                                                
    // float t2 = Age / 1000.0;                                                    
     vec3 DeltaP = 100 * DeltaTimeSecs * in_Velocity0[0];                                 
    // vec3 DeltaV = vec3(DeltaTimeSecs) * (0.0, -9.81, 0.0);  

                                                                                                             
    // out_Position = in_Position[0] + GetRandomDir(u_time);// + DeltaP;                                  
    // out_Velocity = in_Velocity[0];// + vec3(0,1,0);//DeltaV;  
    // if(Age >= 10)
    //     out_Position = vec3(0,0,0);                                
    // out_Age = 0;  

    out_Position = in_Position0[0] + DeltaP;
    out_Velocity = in_Velocity0[0];   
    out_Age = 0;                                               
    EmitVertex();                                                       
    EndPrimitive();                                                                                 
}                                                                                   
