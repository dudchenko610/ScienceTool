#version 430

layout (location = 0) in vec4 VertexPosition;

out vec3 Position;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
    Position = (u_Model * VertexPosition).xyz;
    gl_Position = u_Projection * u_View * u_Model * VertexPosition;
}