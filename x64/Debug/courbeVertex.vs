attribute vec4 a_position;

varying vec3 v_color;

uniform mat4 u_projection;
uniform mat4 u_modelView;

void main(void)
{
	v_color = vec3(1.0,0.0,0.0);
	gl_PointSize = 16.0;
	gl_Position = u_projection * u_modelView * a_position;

	
}