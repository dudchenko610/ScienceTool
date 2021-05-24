#version 430

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VTexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
    Position = (u_Model * VertexPosition).xyz;

    Normal = (u_Model * vec4(VertexNormal, 0.0)).xyz;
    TexCoord = VTexCoord;

    gl_Position = u_Projection * u_View * u_Model * VertexPosition;
}

