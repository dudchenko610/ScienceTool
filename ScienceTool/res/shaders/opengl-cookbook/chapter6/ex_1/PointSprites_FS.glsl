#version 430

in vec2 TexCoord ; // From geometry shader

uniform sampler2D SpriteTex;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture(SpriteTex, TexCoord);
}