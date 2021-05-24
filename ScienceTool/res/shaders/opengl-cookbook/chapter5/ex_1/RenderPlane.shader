#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

out vec2 TexCoord;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	TexCoord = VertexTexCoord;
	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D TexturePlane;

in vec2 TexCoord;

void main()
{
	vec4 texColor = texture(TexturePlane, TexCoord);
	FragColor = texColor;
}