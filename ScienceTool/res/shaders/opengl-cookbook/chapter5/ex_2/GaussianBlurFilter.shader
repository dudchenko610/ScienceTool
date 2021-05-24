#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

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

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

// the texture containing the results of the first pass
layout(binding = 0) uniform sampler2D Texture0;

uniform int PixOffset[5] = int[](0, 1, 2, 3, 4);
uniform float Weight[5];

layout(location = 0) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;

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
	return vec4( phongModel( Position, Normal ), 1.0 );
}

subroutine(RenderPassType)
vec4 pass2()
{
	ivec2 pix = ivec2( gl_FragCoord.xy );
	vec4 sum = texelFetch(Texture0, pix, 0) * Weight[0];

	for (int i = 0; i < 5; i ++)
	{
		sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,  PixOffset[i]) ) * Weight[i];
		sum += texelFetchOffset( Texture0, pix, 0, ivec2(0, -PixOffset[i]) ) * Weight[i];
	}

	return sum;
}

subroutine(RenderPassType)
vec4 pass3()
{
	ivec2 pix = ivec2(gl_FragCoord.xy);
	vec4 sum = texelFetch(Texture0, pix, 0) * Weight[0];

	for (int i = 0; i < 5; i++)
	{
		sum += texelFetchOffset(Texture0, pix, 0, ivec2( PixOffset[i], 0)) * Weight[i];
		sum += texelFetchOffset(Texture0, pix, 0, ivec2(-PixOffset[i], 0)) * Weight[i];
	}

	return sum;
}

void main()
{
	// this will call either pass1(), pass2() or pass3()
	FragColor = RenderPass();
}