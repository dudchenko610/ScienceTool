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

layout(location = 0) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

// the texture containing the results of the preceding pass
uniform sampler2D InputTexture;
uniform sampler2D HighlightResultTexture;

// this subroutine is used for selecting the functionality
subroutine vec4 RenderPassType();  // function reference
subroutine uniform RenderPassType RenderPass; // actual var



// SHADING MODEL
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


// EDGE DETECTING
uniform float LumThresh;
const vec3 lum = vec3(0.2126, 0.7152, 0.0722);
float luminance(vec3 color)
{
	return dot(lum, color);
}

// GAUSSIAN FILTER
uniform int PixOffset[5] = int[](0, 1, 2, 3, 4);
uniform float Weight[5];


// HDR 
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

// Draw scene
subroutine(RenderPassType)
vec4 pass1()
{
	return vec4(phongModel(Position, Normal), 1.0);
}


// Find highlights
subroutine(RenderPassType)
vec4 pass2()
{
	vec4 val = texture(InputTexture, TexCoord);
	if ( luminance(val.rgb) > LumThresh )
	{
		return val;
	}
	else
	{
		return vec4(0.0);
	}
}

// Gaussian blur filter (1 step - sum over columns)
subroutine(RenderPassType)
vec4 pass3()
{
	ivec2 pix = ivec2(gl_FragCoord.xy);
	vec4 sum = texelFetch(InputTexture, pix, 0) * Weight[0];

	for (int i = 0; i < 5; i++)
	{
		sum += texelFetchOffset(InputTexture, pix, 0, ivec2(0, PixOffset[i])) * Weight[i];
		sum += texelFetchOffset(InputTexture, pix, 0, ivec2(0, -PixOffset[i])) * Weight[i];
	}

	return sum;
}

// Gaussian blur filter (2 step - sum over rows)
subroutine(RenderPassType)
vec4 pass4()
{
	ivec2 pix = ivec2(gl_FragCoord.xy);
	vec4 sum = texelFetch(InputTexture, pix, 0) * Weight[0];

	for (int i = 0; i < 5; i++)
	{
		sum += texelFetchOffset(InputTexture, pix, 0, ivec2(PixOffset[i], 0)) * Weight[i];
		sum += texelFetchOffset(InputTexture, pix, 0, ivec2(-PixOffset[i], 0)) * Weight[i];
	}

	return sum;
}

// Apply HDR model to high-resolution texture and plus with highlight result texture
subroutine(RenderPassType)
vec4 pass5()
{
	// Retrieve high-res color from texture
	vec4 color = texture(InputTexture, TexCoord);

	// Convert to XYZ
	vec3 xyzCol = rgb2xyz * vec3(color);

	// Convert to xyY
	float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
	vec3 xyYCol = vec3(0.0);
	if (xyzSum > 0.0)  // Avoid devide by zero
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
	vec4 toneMapColor = vec4(xyz2rgb * xyzCol, 1.0);

	///////// Combine with blurred texture //////////
	vec4 blurColor = texture(HighlightResultTexture, TexCoord);

	return toneMapColor + blurColor;
}

void main()
{
	// this will call one of the passes (1 - 5)
	FragColor = RenderPass();
}