#version 430

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_Size;
layout (location = 2) in vec4 a_Color;
layout (location = 3) in float a_ParentIndex;


//out vec3 Color;
out vec2 Size;

out flat uint ParentIndex;
out flat uint VertexIndex;

void main()
{
  //  Color = a_Color;
    Size = a_Size;

    ParentIndex = uint(a_ParentIndex);
    VertexIndex = uint(gl_VertexID);

    gl_Position = vec4(a_Position, 0.0, 1.0);
}

