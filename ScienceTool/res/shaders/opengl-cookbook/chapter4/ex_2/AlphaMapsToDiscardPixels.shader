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

layout(binding = 0) uniform sampler2D BaseTex;
layout(binding = 1) uniform sampler2D AlphaTex;

layout(location = 0) out vec4 FragColor;

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

vec3 phongModel(vec3 pos, vec3 norm)
{
	vec3 n = normalize(norm);
	vec3 s = normalize(Light.position - Position);
	vec3 v = normalize(u_CameraPos - Position);

	vec3 r = reflect(-s, n);

	return Light.intensity * (
		Ka +
		Kd * max(dot(s, n), 0.0) + 
		Ks * pow(max(dot(r, v), 0.0), Shininess)
	);
}

// If your alpha map has a gradual change in the alpha throughout the map, (for
// example, an alpha map where the alpha values make a smoothly varying height field) then
// it can be used to animate the decay of an object.We could vary the alpha threshold(0.15 in
// the preceding example) from 0.0 to 1.0 to create an animated effect of the object gradually
// decaying away to nothing.

void main()
{

	vec4 baseColor = texture(BaseTex, TexCoord);
	vec4 alphaMap  = texture(AlphaTex, TexCoord);


	if (alphaMap.a <= 0.15)
	{
		discard;
	}
	else 
	{
		if ( gl_FrontFacing ) 
		{
			FragColor = vec4(phongModel(Position, Normal), 1.0) * baseColor;
		}
		else 
		{
			FragColor = vec4(phongModel(Position, -Normal), 1.0) * baseColor;
		}
	}

	

	

}