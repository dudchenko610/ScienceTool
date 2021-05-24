#version 430

layout ( location = 0 ) out vec4 FragColor;

noperspective in vec3 EdgeDistance;
in vec3 Normal;
in vec4 Position;


uniform float LineWidth;
uniform vec4 LineColor;

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

	if (gl_FrontFacing) 
	{
		for (int i = 0; i < 2; i ++) 
		{
			color += ads(i, vec3(Position.xyz), -Normal);
		}
	}
	else
	{
		for (int i = 0; i < 2; i ++) 
		{
			color += ads(i, vec3(Position.xyz), Normal);
		}
	}

	

	return color;
}


float edgeMix()
{
    // Find the smallest distance
    float d = min( min( EdgeDistance.x, EdgeDistance.y ), EdgeDistance.z );

    if( d < LineWidth - 1 ) {
        return 1.0;
    } else if( d > LineWidth + 1 ) {
        return 0.0;
    } else {
        float x = d - (LineWidth - 1);
        return exp2(-2.0 * (x*x));
    }
}


void main()
{

	float mixVal = edgeMix();
    vec4 color = vec4( shadeFragment() , 1.0);
    color = pow( color, vec4(1.0/2.2) );
    //FragColor = mix( color, LineColor, mixVal );
    FragColor = color;

}



