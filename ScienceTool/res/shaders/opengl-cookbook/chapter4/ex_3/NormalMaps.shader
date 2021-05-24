#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;


void main()
{

	TexCoord = VertexTexCoord;
	Position = VertexPosition;
	Normal = VertexNormal;

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D ColorTex;
layout(binding = 1) uniform sampler2D NormalMapTex;


in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;


struct LightInfo {
	vec3 position;
	vec3 intensity; // A, D, S intensity
};

uniform LightInfo Light;

uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

uniform vec3 u_CameraPos;

vec3 phongModel(vec3 normal, vec3 diffR)
{
	vec3 s = normalize(Light.position - Position); // light direction
	vec3 v = normalize(u_CameraPos - Position); // view direction
	vec3 r = reflect(-s, normal);

	float sDotN = max(dot(s, normal), 0.0);
	vec3 diffuse = Light.intensity * diffR * sDotN;

	vec3 ambient = Light.intensity * Ka;

	vec3 spec = vec3(0.0);

	if (sDotN > 0.0) 
	{
		spec = Light.intensity * Ks *
			pow(max(dot(r, v), 0.0), Shininess);
	}
		
	return ambient + diffuse + spec;
}


void main()
{

	vec4 normal = 2.0 * texture(NormalMapTex, TexCoord) - 1.0;

	vec4 texColor = texture(ColorTex, TexCoord);

	//FragColor = vec4( phongModel(Normal, texColor.rgb), 1.0 );
	FragColor = vec4( phongModel( normalize(Normal + vec3(normal)), texColor.rgb), 1.0 );

}