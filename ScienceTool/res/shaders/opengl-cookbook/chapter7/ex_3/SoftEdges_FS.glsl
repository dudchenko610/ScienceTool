#version 430

// Declare any uniforms needed for your shading model
uniform sampler2DShadow ShadowMap;

uniform sampler3D OffsetTex;
uniform vec3 OffsetTexSize; // (width, height, depth)
uniform float Radius;

in vec3 Position;
in vec3 Normal;
in vec4 ShadowCoord;

layout(location = 0) out vec4 FragColor;


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
vec3 diffAndSpec(vec3 position, vec3 normal)
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

	return vec3(diffuse + specular);
}


subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
	vec3 ambient = Light.La * Material.Ka;
	vec3 diffSpec = diffAndSpec(Position, Normal);


	 ivec3 offsetCoord;
	 offsetCoord.xy = ivec2( mod( gl_FragCoord.xy, OffsetTexSize.xy ) );


	 float sum = 0.0;
	 int samplesDiv2 = int(OffsetTexSize.z);
	 vec4 sc = ShadowCoord;

	 for( int i = 0 ; i < 4; i++ ) {

		offsetCoord.z = i;
		vec4 offsets = texelFetch(OffsetTex, offsetCoord, 0) * Radius * ShadowCoord.w;
		
		sc.xy = ShadowCoord.xy + offsets.xy;
		sum += textureProj(ShadowMap, sc);
		
		sc.xy = ShadowCoord.xy + offsets.zw;
		sum += textureProj(ShadowMap, sc);
	 }

	 float shadow = sum / 8.0;
	 if( shadow != 1.0 && shadow != 0.0 ) {

		 for( int i = 4; i< samplesDiv2; i++ ) {

			 offsetCoord.z = i;
			 vec4 offsets = texelFetch(OffsetTex, offsetCoord,0) * Radius * ShadowCoord.w;
			 
			 sc.xy = ShadowCoord.xy + offsets.xy;
			 sum += textureProj(ShadowMap, sc);
			 
			 sc.xy = ShadowCoord.xy + offsets.zw;
			 sum += textureProj(ShadowMap, sc);
		 }

		shadow = sum / float(samplesDiv2 * 2.0);
	 }
 

	// If the fragment is in shadow, use ambient light only
	FragColor = vec4(diffSpec * shadow + ambient, 1.0);

}

subroutine (RenderPassType)
void recordDepth()
{
	// Do nothing, depth will be written automatically

/*	vec3 ambient = Light.La * Material.Ka;
	vec3 diffSpec = diffAndSpec(Position, Normal);

	FragColor = vec4(diffSpec + ambient, 1.0);*/
}

void main()
{
	// This will call either shadeWithShadow or recordDepth
	RenderPass();
}

