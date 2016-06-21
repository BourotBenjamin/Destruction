#version 400                                                                        
                                                                                    
layout (location = 0) in vec3 a_position;                                             
layout (location = 1) in vec3 a_velocity;                                             
layout (location = 2) in float a_age;


                                                                                    
void main()                                                                         
{                                                                                   
    gl_Position = vec4(a_position, 1.0);                                              
}                                                                                   
