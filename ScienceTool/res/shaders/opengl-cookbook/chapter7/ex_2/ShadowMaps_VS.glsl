#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 Position;
out vec3 Normal;

out vec4 ShadowCoord;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat4 ShadowMatrix;

void main()
{

	Position = (u_Model * vec4(VertexPosition, 1.0)).xyz;
	Normal   = (u_Model * vec4(VertexNormal, 0.0)).xyz;

	// ShadowMatrix converts from modeling coordinates
	// to shadow map coordinates
	ShadowCoord = ShadowMatrix * vec4(VertexPosition, 1.0);

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}