#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

out vec3 FrontColor;
out vec3 BackColor;
out vec2 TexCoord;

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

void main()
{

	FrontColor = phongModel(VertexPosition, VertexNormal);
	BackColor  = phongModel(VertexPosition, -VertexNormal);
	TexCoord   = VertexTexCoord;

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

in vec3 FrontColor;
in vec3 BackColor;
in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;

void main()
{

	const float scale = 20.0;

	bvec2 toDiscard = greaterThan(fract(TexCoord * scale), vec2(0.1, 0.1));

	if (all(toDiscard)) {
		discard;
	}

	if (gl_FrontFacing) {
		FragColor = vec4(FrontColor, 1.0);
	}
	else {
		//FragColor = mix(vec4(BackColor, 1.0), vec4(0.0, 0.0, 1.0, 1.0), 0.5);
		FragColor = vec4(BackColor, 1.0);
	}


}