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

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D Tex1;

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

void phongModel(vec3 pos, vec3 norm, out vec3 ambAndDiff, out vec3 spec)
{
	vec3 n = normalize(norm);
	vec3 s = normalize(Light.position - Position);
	vec3 v = normalize(u_CameraPos - Position);

	vec3 r = reflect(-s, n);

	ambAndDiff = Light.intensity * (
		Ka +
		Kd * max(dot(s, n), 0.0));

	spec = Light.intensity * Ks * pow(max(dot(r, v), 0.0), Shininess);
}

const vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);

void main()
{

	vec4 texColor = texture(Tex1, TexCoord);

	vec3 ambAndDiff, spec;
	phongModel(Position, Normal, ambAndDiff, spec);


	/*texColor = vec4(0.0, 0.0, 0.0, 1.0);

	if (TexCoord.s > 1.0)
		texColor = yellow;
		*/

	FragColor = vec4(ambAndDiff, 1.0) * texColor + vec4(spec, 1.0);
	//FragColor = texColor;

}