#shader vertex
#version 430

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

out vec4 v_color;

void main()
{
	v_color = color;
	gl_Position = vec4(position.xy, 0, 1.0);
}


#shader fragment
#version 430

layout(location = 0) out vec4 color;

in vec4 v_color;

void main()
{
	color = v_color;
}