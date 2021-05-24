#version 430

layout(location = 0) out vec4 FragColor;

in vec3 GPosition;
in vec3 GNormal;
noperspective in vec3 GEdgeDistance;

// The mesh line settings
uniform struct LineInfo 
{
	float Width;
	vec4 Color;
} Line;



// SHADE MODEL
struct LightInfo {
	vec3 Position;       // light position
	vec3 Intensity;
};

uniform LightInfo lights[5];

uniform vec3 Ka;		 // ambient reflectivity
uniform vec3 Kd;		 // diffuse reflectivity
uniform vec3 Ks;		 // specular reflectivity
uniform float Shininess; // specular shininess factor

uniform vec3 u_CameraPos;

vec3 ads(int lightIndex, vec3 position, vec3 normal)
{
	vec3 s = normalize( lights[lightIndex].Position - position );
	vec3 v = normalize( u_CameraPos - position );
	vec3 r = reflect( -s, normal );
	vec3 I = lights[lightIndex].Intensity;

	return I * (Ka + 
		Kd * max( dot( s, normal ), 0.0 ) +
		Ks * pow( max( dot( r, v ), 0.0 ), Shininess));
}

vec3 shadeFragment() 
{
	vec3 color = vec3(0.0);

	for (int i = 0; i < 5; i ++) 
	{
		color += ads(i, GPosition, GNormal);
	}

	return color;
}

void main()
{
	// The shaded surface color
	vec4 color = vec4(shadeFragment(), 1.0);

	// Find the smallest distance
	float d = min(GEdgeDistance.x, GEdgeDistance.y);
	d = min(d, GEdgeDistance.z);

	// Determine the mix factor with line color
	float mixVal = smoothstep(Line.Width - 1, Line.Width + 1, d);

	// Mix the surface color with the line color
	vec4 res =  mix(Line.Color, color, mixVal);
	res.a = 1.0;
	FragColor = res;
}