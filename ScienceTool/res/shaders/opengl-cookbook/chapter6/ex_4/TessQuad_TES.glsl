#version 430

layout( quads, equal_spacing, ccw ) in;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main() 
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;

	// Linear interpolation
	gl_Position = p0 * ( 1 - u ) * ( 1 - v ) + 
				  p1 * u * ( 1 - v ) +
				  p3 * v * ( 1 - u ) +
				  p2 * u * v;

	// Transform to clip coordinates
	gl_Position = u_Projection * u_View * u_Model * gl_Position;

}

