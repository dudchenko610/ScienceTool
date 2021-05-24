#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;


out vec3 Position;
out vec3 Normal;

uniform float Time; // The animation time

// Wave parameters
uniform float K;        // Wavenumber
uniform float Velocity; // Wave's velocity
uniform float Amp;      // Wave's amplitude

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	

	vec4 pos = vec4(VertexPosition, 1.0);
	
	// Translate the y coordinate
	float u = K * (pos.x - Velocity * Time);
	pos.y   = Amp * sin( u );

	
	// Compute the normal vector
	vec3 n = vec3(0.0);
	n.xy = normalize( vec2( -K * Amp * cos(u), 1.0 ) );
	
	Position = vec3(pos);
	Normal   = n;

	Position = pos.xyz;

	gl_Position = u_Projection * u_View * u_Model * vec4(pos.xyz, 1.0);
}