#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 Position;
out vec3 Normal;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;


void main()
{

	Position = VertexPosition;
	Normal = VertexNormal;

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

layout(location = 0) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;

struct LightInfo {
	vec3 position;
	vec3 intensity;
};

uniform LightInfo Light;

uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity

const int levels = 4;
const float scaleFactor = 1.0 / levels;

vec3 toonShade()
{
	vec3 s = normalize( Light.position - Position );
	float cosine = max( 0.0, dot(s, Normal ) );
	vec3 diffuse = Kd * floor( cosine * levels ) * scaleFactor;

	return Light.intensity * ( Ka + diffuse );
}

void main()
{
	FragColor = vec4(toonShade(), 1.0);
}