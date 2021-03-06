#version 430

layout(vertices = 4) out;

uniform int NumSegments;
uniform int NumStrips;

void main()
{
	// Pass along the vertex position unmodified
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	// Define the tessellation levels
	gl_TessLevelOuter[0] = float(NumStrips);
	gl_TessLevelOuter[1] = float(NumSegments);
}


//Note that the values for gl_TessLevelOuter are floating point numbers rather than integers