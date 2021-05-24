#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

// the g-buffer textures
layout(binding = 0) uniform sampler2D PositionTex;
layout(binding = 1) uniform sampler2D NormalTex;
layout(binding = 2) uniform sampler2D ColorTex;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 PositionData;
layout(location = 2) out vec3 NormalData;
layout(location = 3) out vec3 ColorData;


// this subroutine is used for selecting the functionality
subroutine void RenderPassType();  // function reference
subroutine uniform RenderPassType RenderPass; // actual var



// SHADING MODEL
struct LightInfo {
	vec3 Position;
	vec3 Intensity; // A, D, S intensity
};
struct MaterialInfo {
	vec3 Kd;
};
uniform LightInfo Light;
uniform MaterialInfo Material;
uniform vec3 u_CameraPos;

vec3 diffuseModel(vec3 pos, vec3 norm, vec3 diff)
{
	vec3 s = normalize(vec3(Light.Position) - pos);
	float sDotN = max(dot(s, norm), 0.0);
	vec3 diffuse = Light.Intensity * diff * sDotN;
	return diffuse;
}



// Draw scene
subroutine(RenderPassType)
void pass1()
{
	// Store, position, norm and diffuse color in g-buffer
	PositionData = Position;
	NormalData = Normal;
	ColorData = Material.Kd;
}


// Apply shading model
subroutine(RenderPassType)
void pass2()
{
	// Retrieve position, normal and color information from the g-buffer textures
	vec3 pos = vec3(texture(PositionTex, TexCoord));
	vec3 norm = vec3(texture(NormalTex, TexCoord));
	vec3 diffColor = vec3(texture(ColorTex, TexCoord));

	FragColor = vec4( diffuseModel(pos, norm, diffColor), 1.0 );
}


void main()
{
	// this will call one of the passes (1 - 2)
	RenderPass();
}