#version 430

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

//in vec3[] Color;
in vec2[] Size;
//in flat uint[] Index;
in flat uint[] VertexIndex;
in flat uint[] ParentIndex;

out vec2 v_TexCoord;
//out vec3 v_Color;

out flat uint v_VertexIndex;
out flat uint v_ParentIndex;


uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
//	v_Color = Color[0];
//	v_Index = Index[0];
	v_VertexIndex = VertexIndex[0];
	v_ParentIndex = ParentIndex[0];

	mat4 m = u_Projection * u_View * u_Model;

	float halfWidth  = Size[0].x * 0.5;
	float halfHeight = Size[0].y * 0.5;

	gl_Position = m * (vec4(-halfWidth, -halfHeight, 0.0, 0.0) + gl_in[0].gl_Position);
	v_TexCoord = vec2(0.0, 0.0);
	EmitVertex();

	gl_Position = m * (vec4(halfWidth, -halfHeight, 0.0, 0.0) + gl_in[0].gl_Position);
	v_TexCoord = vec2(1.0, 0.0);
	EmitVertex();

	gl_Position = m * (vec4(-halfWidth, halfHeight, 0.0, 0.0) + gl_in[0].gl_Position);
	v_TexCoord = vec2(0.0, 1.0);
	EmitVertex();

	gl_Position = m * (vec4(halfWidth, halfHeight, 0.0, 0.0) + gl_in[0].gl_Position);
	v_TexCoord = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive();
}