#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 Position;
out vec3 Normal;
out vec4 ProjTexCoord;

// page 151
uniform mat4 ProjectorMatrix; // m-matrix = scaleTrans(0.5) * proj * view;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;


void main()
{

	vec4 pos4 = vec4(VertexPosition, 1.0);

	Position = VertexPosition;
	Normal = VertexNormal;
	ProjTexCoord = ProjectorMatrix * (u_Model * pos4 );

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

layout(location = 0) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec4 ProjTexCoord;

layout(binding = 0) uniform sampler2D ProjectorTex;

struct MaterialInfo {
	vec3 Kd;
	vec3 Ks;
	vec3 Ka;
	float Shininess;
};
uniform MaterialInfo Material;


struct LightInfo {
	vec3 Position;
	vec3 Intensity; // A, D, S intensity
};

uniform LightInfo Light;

uniform vec3 u_CameraPos;

vec3 phongModel(vec3 pos, vec3 norm)
{
	vec3 s = normalize(Light.Position - Position);
	vec3 v = normalize(u_CameraPos - Position);
	vec3 r = reflect(-s, norm);

	vec3 ambient = Light.Intensity * Material.Ka;
	float sDotN = max( dot(s, norm), 0.0 );
	vec3 diffuse = Light.Intensity * Material.Kd * sDotN;

	vec3 spec = vec3(0.0);
	if (sDotN > 0.0)
	{
		spec = Light.Intensity * Material.Ks *
			pow(max(dot(r, v), 0.0), Material.Shininess);
	}

	return ambient + diffuse + spec;

}

void main()
{

	vec3 color = phongModel(Position, Normal);

	vec4 projTexColor = vec4(0.0);
	if (ProjTexCoord.z > 0.0) 
	{
		projTexColor = textureProj(ProjectorTex, ProjTexCoord);
	}

	FragColor = vec4(color, 1.0) + projTexColor * 0.5;

}