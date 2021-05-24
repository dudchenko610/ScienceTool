#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 ReflectDir; // reflected direction
out vec3 RefractDir; // transmitted direction

struct MaterialInfo {
	float Eta;              // Ratio of indices of refraction
	float ReflectionFactor; // 
};

uniform MaterialInfo Material;

uniform bool DrawSkyBox;
uniform vec3 u_CameraPos;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;


void main()
{

	if (DrawSkyBox)
	{
		ReflectDir = VertexPosition;
	}
	else
	{
		vec3 v = u_CameraPos - VertexPosition;

		ReflectDir = reflect(-v, VertexNormal);
		RefractDir = refract(-v, VertexNormal, Material.Eta);
	}

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform samplerCube CubeMapTex;

in vec3 ReflectDir; // reflected direction
in vec3 RefractDir; // transmitted direction

struct MaterialInfo {
	float Eta;              // Ratio of indices of refraction
	float ReflectionFactor; // 
};

uniform MaterialInfo Material;

uniform bool DrawSkyBox;
uniform vec4 MaterialColor; // Color of the object's "tint"

void main()
{
	// Access the cube map texture
	vec4 reflectColor = texture(CubeMapTex, ReflectDir);
	vec4 refractColor = texture(CubeMapTex, RefractDir);

	if (DrawSkyBox)
	{
		FragColor = reflectColor;
	}
	else
	{
		FragColor = mix(refractColor, reflectColor, Material.ReflectionFactor);
	}

}