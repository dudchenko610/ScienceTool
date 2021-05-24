#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 _TexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	Position = vec3(u_Model * vec4(VertexPosition, 1.0));
	Normal = vec3(u_Model * vec4(VertexNormal, 0.0));
	TexCoord = _TexCoord;

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

// the texture containing the results of the first pass
layout(binding = 0) uniform sampler2D HdrTex;

layout(location = 0) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform float AveLum;

uniform mat3 rgb2xyz = mat3(
	0.4124564, 0.2126729, 0.0193339,
	0.3575761, 0.7151522, 0.1191920,
	0.1804375, 0.0721750, 0.9503041);

uniform mat3 xyz2rgb = mat3(
	3.2404542, -0.9692660, 0.0556434,
	-1.5371385, 1.8760108, -0.2040259,
	-0.4985314, 0.0415560, 1.0572252);

uniform float Exposure = 0.35;
uniform float White = 0.928;

// this subroutine is used for selecting the functionality
subroutine vec4 RenderPassType();  // function reference
subroutine uniform RenderPassType RenderPass; // actual var

struct LightInfo {
	vec3 Position;
	vec3 Intensity; // A, D, S intensity
};

struct MaterialInfo {
	vec3 Kd;
	vec3 Ks;
	vec3 Ka;
	float Shininess;
};

uniform LightInfo Light;
uniform MaterialInfo Material;
uniform vec3 u_CameraPos;


vec3 phongModel(vec3 pos, vec3 norm)
{
	vec3 s = normalize(Light.Position - Position);
	vec3 v = normalize(u_CameraPos - Position);
	vec3 r = reflect(-s, norm);

	vec3 ambient = Light.Intensity * Material.Ka;
	float sDotN = max(dot(s, norm), 0.0);
	vec3 diffuse = Light.Intensity * Material.Kd * sDotN;

	vec3 spec = vec3(0.0);
	if (sDotN > 0.0)
	{
		spec = Light.Intensity * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
	}

	return ambient + diffuse + spec;

}


subroutine(RenderPassType)
vec4 pass1()
{
	return vec4(phongModel(Position, Normal), 1.0);
}

subroutine(RenderPassType)
vec4 pass2()
{
	// Retrieve high-res color from texture
	vec4 color = texture( HdrTex, TexCoord );

	// Convert to XYZ
	vec3 xyzCol = rgb2xyz * vec3(color);

	// Convert to xyY
	float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
	vec3 xyYCol = vec3(0.0);
	if ( xyzSum > 0.0 )  // Avoid devide by zero
	{
		xyYCol = vec3(xyzCol.x / xyzSum,
			xyzCol.y / xyzSum, xyzCol.y);
	}

	// Apply the tone mapping operation to the luminance 
	// (xyYCol.z or xyzCol.y)
	float L = (Exposure * xyYCol.z) / AveLum;
	L = (L * (1 + L / (White * White))) / (1 + L);

	// Using the new luminance, convert back to XYZ
	if (xyYCol.y > 0.0) {
		xyzCol.x = (L * xyYCol.x) / (xyYCol.y);
		xyzCol.y = L;
		xyzCol.z = (L * (1 - xyYCol.x - xyYCol.y)) / xyYCol.y;
	}

	// Convert back to RGB and send to output buffer
	return vec4(xyz2rgb * xyzCol, 1.0);
}

void main()
{
	// this will call either pass1() or pass2()
	FragColor = RenderPass();
}