#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 Color;


struct LightInfo {
	vec4 Position;       // light position
	vec3 Intensity;
};

uniform LightInfo lights[1];

uniform vec3 Ka;		 // ambient reflectivity
uniform vec3 Kd;		 // diffuse reflectivity
uniform vec3 Ks;		 // specular reflectivity
uniform float Shininess; // specular shininess factor

uniform vec3 u_CameraPos;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

vec3 ads(int lightIndex, vec3 position, vec3 normal)
{
	vec3 s;
	if (lights[lightIndex].Position.w == 0.0)
	{
		s = normalize(vec3(lights[lightIndex].Position));
	}
	else
	{
		s = normalize(vec3(lights[lightIndex].Position) - position);
	}
		
	vec3 v = normalize( u_CameraPos - position );
	vec3 r = reflect( -s, normal );
	vec3 I = lights[lightIndex].Intensity;

	return I * (Ka + 
		Kd * max( dot( s, normal ), 0.0 ) +
		Ks * pow( max( dot( r, v ), 0.0 ), Shininess));
}

void main()
{

	// evaluate the lighting equation for each light
	Color = vec3(0.0);

	for (int i = 0; i < 1; i ++) 
	{
		Color += ads(i, VertexPosition, VertexNormal);
	}

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

layout(location = 0) out vec4 FragColor;

in vec3 Color;

void main()
{
	FragColor = vec4(Color, 1.0);
}