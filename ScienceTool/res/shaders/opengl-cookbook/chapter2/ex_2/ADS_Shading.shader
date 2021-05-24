#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 LightIntensivity;

struct LightInfo {
	vec3 Position; // light position
	vec3 La;       // ambient light intensity
	vec3 Ld;       // diffuse light intensity
	vec3 Ls;       // specular light intensity;
};

uniform LightInfo Light;

struct MaterialInfo {
	vec3 Ka;		 // ambient reflectivity
	vec3 Kd;		 // diffuse reflectivity
	vec3 Ks;		 // specular reflectivity
	float Shininess; // specular shininess factor
};


uniform MaterialInfo Material;

uniform vec3 u_CameraPos;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	mat4 MV_matrix = u_View * u_Model;
	vec4 vertPosEyeCoords = MV_matrix * (vec4(VertexPosition, 1.0));

	vec3 s = normalize(Light.Position - VertexPosition);
	vec3 v = normalize(u_CameraPos - VertexPosition);
	vec3 r = reflect(-s, VertexNormal);

	float sDotN = max(dot(s, VertexNormal), 0.0);

	vec3 ambient  = Light.La * Material.Ka;
	vec3 diffuse  = Light.Ld * Material.Kd * sDotN;
	vec3 specular = vec3(0.0);

	if (sDotN > 0.0)
	{
		specular = Light.Ls * Material.Ks 
			* pow(max( dot(r, v), 0.0 ), Material.Shininess);
	}

	LightIntensivity = ambient + diffuse + specular;

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

in vec3 LightIntensivity;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(LightIntensivity, 1.0);
}