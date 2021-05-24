#version 430

layout(location = 0) in vec3 VertexPosition;


uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	gl_Position = u_View * u_Model * vec4(VertexPosition, 1.0);
}
