#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 ReflectedDir; // the direction of the reflected ray

uniform bool DrawSkyBox;
uniform vec3 u_CameraPos;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;


void main()
{

	if ( DrawSkyBox )
	{
		ReflectedDir = VertexPosition;
	}
	else
	{
		vec3 v = u_CameraPos - VertexPosition;
		ReflectedDir = reflect(-v, VertexNormal);
	}

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}

#shader fragment
#version 430

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform samplerCube CubeMapTex;

in vec3 ReflectedDir;  // the direction of the reflected ray

uniform bool DrawSkyBox;
uniform float ReflectFactor;
uniform vec4 MaterialColor; // Color of the object's "tint"

void main()
{

	vec4 cubeMapColor = texture(CubeMapTex, ReflectedDir);

	if ( DrawSkyBox )
	{
		FragColor = cubeMapColor;
	}
	else
	{
		FragColor = mix(MaterialColor, cubeMapColor, ReflectFactor);
	}

}