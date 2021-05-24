#shader vertex
#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

out vec3 LightIntensivity;

uniform vec3 LightPosition;
uniform vec3 Kd;  // Diffuse reflectivity
uniform vec3 Ld;  // Light source intensivity


void main()
{

	vec3 s = normalize(vec3(LightPosition - position)); // vertex-lightPos vector
	
	// The diffuse shading equation
	LightIntensivity = Ld * Kd * max( dot( s, normal ), 0.1 );

	gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0);
}


#shader fragment
#version 430

layout(location = 0) out vec4 FragColor;

in vec3 LightIntensivity;

void main()
{

	FragColor = vec4(LightIntensivity, 1.0);
}