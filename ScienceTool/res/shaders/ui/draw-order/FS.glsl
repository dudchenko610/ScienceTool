#version 430

//in vec3 v_Color;

in flat uint v_VertexIndex;
in flat uint v_ParentIndex;

uniform int Width = 1000;
uniform uint maxuint = 0xFFFFFFFF;

layout( location = 0 ) out vec4 FragColor;

layout(std430, binding = 0) buffer VertexBuffer {
    float Vertices[];
};

layout(std430, binding = 1) buffer CounterBuffer {
    uvec4 Counters[];
};


subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;


subroutine(RenderPassType)
void pass1()
{

/*
    uint ind = (uint(v_VertexIndex) * 9);
    uint colorIndex = ind + 4;

    vec4 fragCol = vec4(Vertices[colorIndex + 0], Vertices[colorIndex + 1], Vertices[colorIndex + 2], Vertices[colorIndex + 3]);

    FragColor = fragCol;
    */

	
    ivec2 fragCoords        = ivec2(gl_FragCoord);
    int   thisPixelIndex    = fragCoords.y * Width + fragCoords.x;
    uvec4 curPix            = Counters[thisPixelIndex];
    
    
    if (v_ParentIndex == maxuint) // it is a parent
    {
        
        bool written = false;

        for (int i = 0; i < 4; i ++) 
        {
            if (curPix[i] == v_VertexIndex) // it is already in pixel due to multisamling
            {
                written = true;
                break;
            }

            if (curPix[i] == maxuint)
            {
                written = true;
                curPix[i] = v_VertexIndex; // parent's vertex index
                Counters[thisPixelIndex] = curPix; // actual write to SSBO
                break;
            }
        }

        if (!written)
        {
            // shift elements and write, we loose r-component
            curPix.r = curPix.g;
            curPix.g = curPix.b;
            curPix.b = curPix.a;


            curPix.a = v_VertexIndex; // parent's vertex index
            Counters[thisPixelIndex] = curPix; // actual write to SSBO

        }
        
    }
    else // it is a child
    {
        
        bool hasParent = false;

        uint ind = -1;

        for (int i = 0; i < 4; i ++) 
        {
             if (curPix[i] == v_VertexIndex) // it is already in pixel due to multisamling
             {
                hasParent = false;
                break;
             }

             if (curPix[i] == v_ParentIndex) 
             {
                hasParent = true;
             }

             if (curPix[3 - i] == maxuint) 
             {
                ind = 3 - i;
             }
        }


        if (hasParent)
        {
            if (ind == -1) // there no empty space
            {
                // shift elements and write, we loose r-component
                curPix.r = curPix.g;
                curPix.g = curPix.b;
                curPix.b = curPix.a;

                curPix.a = v_VertexIndex;
            }
            else 
            {
                curPix[ind] = v_VertexIndex;
            }

            Counters[thisPixelIndex] = curPix;

        }
       
    }
    
   

}

subroutine(RenderPassType)
void pass2()
{
    ivec2 fragCoords        = ivec2(gl_FragCoord);
    int   thisPixelIndex    = fragCoords.y * Width + fragCoords.x;
    uvec4 curPix            = Counters[thisPixelIndex];

    
    if (curPix.r == maxuint) 
    {
        discard;
    } 
    else 
    {
	    vec4 color = vec4(0.0, 0.0, 0.0, 1.0); // Background color
    

        for (int i = 0; i < 4; i ++) 
        {

            if (curPix[i] == maxuint) 
            {
                continue;
            }

            uint ind = (uint(curPix[i]) * 9);
            uint colorIndex = ind + 4;

            vec4 fragCol = vec4(Vertices[colorIndex + 0], Vertices[colorIndex + 1], Vertices[colorIndex + 2], Vertices[colorIndex + 3]);
            color = mix(color, fragCol, fragCol.a);
        }

        FragColor = color;
    }
        
}



void main() 
{
    RenderPass();
}