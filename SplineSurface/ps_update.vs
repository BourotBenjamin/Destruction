#version 400                                                                        
                                                                                    
layout (location = 0) in vec3 a_position;                                             
layout (location = 1) in vec3 a_velocity;                                             
layout (location = 2) in float a_age;                                                 
    
// out VS_OUT {                                                                       
// 	vec3 Position;                                                                 
// 	vec3 Velocity;                                                                 
// 	float Age;    
// }vs_out;  

// out	vec3 in_Position;                                                                 
// out	vec3 in_Velocity;                                                                 
// out float in_Age; 

// void main()                                                                         
// {                                         
// 	vec3 velocity = vec3(0,0.1,0);

//     in_Position = a_position + a_velocity ;                                                           
//     in_Velocity = a_velocity + velocity;                                                           
//     in_Age = a_age;                                                                     
// }

out	vec3 in_Position0;                                                                 
out	vec3 in_Velocity0;                                                                 
out float in_Age0; 

void main()                                                                         
{                                                                                   
    in_Position0 = a_position.xyz;                                                           
    in_Velocity0 = a_velocity;                                                           
    in_Age0 = a_age;                                                                     
}