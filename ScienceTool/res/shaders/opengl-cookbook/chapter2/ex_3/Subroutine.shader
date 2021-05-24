#shader vertex
#version 430

subroutine vec3 shadeModelType( vec3 position, vec3 normal ); // like a C-style header
subroutine uniform shadeModelType shadeModel;                 // like a C-style function pointer

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

subroutine ( shadeModelType )
vec3 phongModel(vec3 position, vec3 normal)
{
	vec3 s = normalize(Light.Position - position);
	vec3 v = normalize(u_CameraPos - position);
	vec3 r = reflect(-s, normal);

	float sDotN = max(dot(s, normal), 0.0);

	vec3 ambient = Light.La * Material.Ka;
	vec3 diffuse = Light.Ld * Material.Kd * sDotN;
	vec3 specular = vec3(0.0);

	if (sDotN > 0.0)
	{
		specular = Light.Ls * Material.Ks
			* pow(max(dot(r, v), 0.0), Material.Shininess);
	}

	return vec3(ambient + diffuse + specular);
}

subroutine ( shadeModelType )
vec3 diffuseOnly(vec3 position, vec3 normal)
{
	vec3 s = normalize(Light.Position - position);
	return Light.Ld * Material.Kd * max(dot(s, normal), 0.0);
}



void main()
{
	// Evaluate the shading equation, calling one of
	// the functions: diffuse only or phongModel

	LightIntensivity = shadeModel(VertexPosition, VertexNormal);

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