#version 400                                                                        
                  
in GS_OUT {                                                                       
    vec2 TexCoord;  
}fs_in; 

uniform sampler2D u_sampler;                                                        
                                                                                      
out vec4 FragColor;                                                                 
                                                                                    
void main()                                                                         
{                                                                                   
    FragColor = vec4(1.0);                                     
    //FragColor = texture2D(u_sampler, fs_in.TexCoord);                                                                               
    //if (FragColor.r >= 0.9 && FragColor.g >= 0.9 && FragColor.b >= 0.9) {           
    //    discard;                                                                    
    //}                                                                               
}