#shader vertex
#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 Position;
out vec3 Normal;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	Position = VertexPosition;
	Normal = VertexNormal;

	gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);
}


#shader fragment
#version 430

layout(location = 0) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;


// SHADE MODEL
struct LightInfo {
	vec3 Position;       // light position
	vec3 Intensity;
};
uniform LightInfo lights[5];
uniform vec4 Ka;		 // ambient reflectivity
uniform vec3 Kd;		 // diffuse reflectivity
uniform vec3 Ks;		 // specular reflectivity
uniform float Shininess; // specular shininess factor
uniform vec3 u_CameraPos;

vec4 ads(int lightIndex, vec3 position, vec3 normal)
{
	vec3 s = normalize(lights[lightIndex].Position - position);
	vec3 v = normalize(u_CameraPos - position);
	vec3 r = reflect(-s, normal);
	vec4 I = vec4(lights[lightIndex].Intensity, 1.0);

	vec4 ambient = Ka;
	vec4 diffuse = vec4(Kd * max(dot(s, normal), 0.0), 0.0);
	vec4 specular = vec4(Ks * pow(max(dot(r, v), 0.0), Shininess), 0.0);

	return I * (ambient + diffuse + specular);
}
vec4 shadeFragment()
{
	// evaluate the lighting equation for each light

	vec4 color = vec4(0.0);

	if (gl_FrontFacing)
	{
		for (int i = 0; i < 2; i++)
		{
			color += ads(i, Position, Normal);
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			color += ads(i, Position, -Normal);
		}
	}

	return color;
}



// DEPTH MODEL
layout(early_fragment_tests) in;

#define MAX_FRAGMENTS 75

struct NodeType {
	vec4 color;
	float depth;
	uint next;
};

layout(binding = 0, r32ui) uniform uimage2D headPointers;
layout(binding = 0, offset = 0) uniform atomic_uint nextNodeCounter;

layout(binding = 0, std430) buffer linkedLists {
	NodeType nodes[];
};

uniform uint MaxNodes;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;


subroutine(RenderPassType)
void pass1()
{
	// Get the index of the next empty slot in the buffer
	uint nodeIdx = atomicCounterIncrement(nextNodeCounter);

	// Is there space left in the buffer
	if ( nodeIdx < MaxNodes )
	{
		// Update the head pointer image
		uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);
		
		// Set the color and depth of this new node to the color
		// and depth of the fragment. The next pointer points to the 
		// previous head of the list
		nodes[nodeIdx].color = shadeFragment();
		nodes[nodeIdx].depth = gl_FragCoord.z;
		nodes[nodeIdx].next  = prevHead;
	}

	//FragColor = shadeFragment();
}

subroutine(RenderPassType)
void pass2()
{
	// We start by copying the linked list for the fragment into a temporary array
	NodeType frags[MAX_FRAGMENTS];
	int count = 0;

	// Get the index of the head of the list
	uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;

	// Copy the linked list for this fragment into an array
	while ( n != 0xffffffff && count < MAX_FRAGMENTS ) 
	{
		frags[count] = nodes[n];
		n = frags[count].next;
		count++;
	}

	// Then, we sort the array by depth (largest to smallest) using insertion sort
	for (uint i = 1; i < count; i ++)
	{
		NodeType toInsert = frags[i];
		uint j = i;

		while (j > 0 && toInsert.depth > frags[j - 1].depth)
		{
			frags[j] = frags[j-1];
			j--;
		}

		frags[j] = toInsert;
	}

	// Finally, we blend the fragments "manually", and send the result to the output variable
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0); // Background color
	for (int i = 0; i < count; i++)
	{
		color = mix(color, frags[i].color, frags[i].color.a);
	}

	// Output the final color
	FragColor = color;
}



void main()
{
	RenderPass();
}