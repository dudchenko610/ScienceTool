#version 430


in vec3 Position;
in vec3 Normal;

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

	if (gl_FrontFacing) 
	{
		FragColor = vec4( phongModel( Position, Normal ), 1.0 );
	}
	else
	{
		discard;
		//FragColor = vec4( phongModel( Position, -Normal ), 1.0 );
	}
	

	//FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );

}

