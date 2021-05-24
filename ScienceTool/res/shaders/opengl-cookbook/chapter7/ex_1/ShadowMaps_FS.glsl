#version 430

// Declare any uniforms needed for your shading model
uniform sampler2DShadow ShadowMap;


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

	// Do the shadow-map look-up
	float shadow = textureProj(ShadowMap, ShadowCoord);

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


// The next step is the key to the shadow mapping algorithm. We use the built-in texture
// access function textureProj, to access the shadow map texture ShadowMap. Before using
// the texture coordinate to access the texture, the textureProj function will divide the first
// three components of the texture coordinate by the fourth component. Remember that this
// is exactly what is needed to convert the homogeneous position (ShadowCoord) to a true
// Cartesian position.
// 
// After this perspective division, the textureProj function will use the result to access
// the texture. As this texture's sampler type is sampler2DShadow, it is treated as texture
// containing depth values, and rather than returning a value from the texture, it returns the
// result of a comparison. The first two components of ShadowCoord are used to access a
// depth value within the texture. That value is then compared against the value of the third
// component of ShadowCoord. When GL_NEAREST is the interpolation mode (as it is in our
// case) the result will be 1.0 or 0.0. As we set the comparison function to GL_LESS, this will
// return 1.0, if the value of the third component of ShadowCoord is less than the value within
// the depth texture at the sampled location. This result is then stored in the variable shadow.
// Finally, we assign a value to the output variable FragColor. The result of the shadow map
// comparison (shadow) is multiplied by the diffuse and specular components, and the result is
// added to the ambient component. If shadow is 0.0, that means that the comparison failed,
// meaning that there is something between the fragment and the light source. Therefore, the
// fragment is only shaded with ambient light. Otherwise, shadow is 1.0, and the fragment is
// shaded with all three shading components.
// When rendering the shadow map, note that we 