#shader vertex
#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

out vec4 v_color;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	v_color = color;
	gl_Position = u_Projection * u_View * vec4(position, 1.0);
}


#shader fragment
#version 430

layout(location = 0) out vec4 FragColor;

in vec4 v_color;

void main()
{
	FragColor = v_color;
}