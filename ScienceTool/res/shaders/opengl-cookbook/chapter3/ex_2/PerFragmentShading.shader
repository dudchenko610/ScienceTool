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

uniform vec3 LightPosition;
uniform vec3 LightIntensity;

uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

uniform vec3 u_CameraPos;

vec3 ads()
{
	vec3 n = normalize(Normal);
	vec3 s = normalize(LightPosition - Position);
	vec3 v = normalize(u_CameraPos - Position);

	vec3 r = reflect( -s, n );

	return LightIntensity * (
		Ka +
		Kd * max( dot(s, n), 0.0) +
		Ks * pow( max( dot( r, v ), 0.0 ), Shininess ));
}

void main()
{
	FragColor = vec4(ads(), 1.0);
}