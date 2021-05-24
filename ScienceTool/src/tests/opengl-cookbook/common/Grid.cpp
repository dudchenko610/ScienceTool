#include "Grid.h"

#include "../../../core/opengl/core/error/ErrorChecker.h"

core::Grid::Grid(bool xzPlane, bool xyPlane, bool zyPlane)
{

	glm::vec4 color = glm::vec4(1.0, 1.0f, 1.0f, 0.1f);

	glm::vec4 xColor = glm::vec4(1.0, 0.0f, 0.0f, 0.7f);
	glm::vec4 yColor = glm::vec4(0.0, 1.0f, 0.0f, 0.7f);
	glm::vec4 zColor = glm::vec4(0.0, 0.0f, 1.0f, 0.7f);

	int min = -50;
	int max = 50;

	int size = max - min;

	int lineCount = (xzPlane * size + xyPlane * size + zyPlane * size) * 2 + 3;
	int floatCount = (3 + 4) * 2 * lineCount; // (pos + color) * 2 points * lineCount
	int byteCount = floatCount * sizeof(float);

	/*std::cout << "lineCount = " << lineCount << std::endl;
	std::cout << "byteCount = " << byteCount << std::endl;
	std::cout << "floatCount = " << floatCount << std::endl;*/

	float* data = new float[floatCount]; //  2 * to create grid
	

	this->vertexCount = lineCount * 2;

	int counter = 0;

	// fill origin lines

	{
		data[counter++] = -200.0f;
		data[counter++] = 0.0f;
		data[counter++] = 0.0f;

		data[counter++] = xColor.x;
		data[counter++] = xColor.y;
		data[counter++] = xColor.z;
		data[counter++] = xColor.w;

		data[counter++] = 200.0f;
		data[counter++] = 0.0f;
		data[counter++] = 0.0f;

		data[counter++] = xColor.x;
		data[counter++] = xColor.y;
		data[counter++] = xColor.z;
		data[counter++] = xColor.w;


	}

	{
		data[counter++] = 0.0f;
		data[counter++] = -200.0f;
		data[counter++] = 0.0f;

		data[counter++] = yColor.x;
		data[counter++] = yColor.y;
		data[counter++] = yColor.z;
		data[counter++] = yColor.w;

		data[counter++] = 0.0f;
		data[counter++] = 200.0f;
		data[counter++] = 0.0f;

		data[counter++] = yColor.x;
		data[counter++] = yColor.y;
		data[counter++] = yColor.z;
		data[counter++] = yColor.w;
	}

	{
		data[counter++] = 0.0f;
		data[counter++] = 0.0f;
		data[counter++] = -200.0f;

		data[counter++] = zColor.x;
		data[counter++] = zColor.y;
		data[counter++] = zColor.z;
		data[counter++] = zColor.w;

		data[counter++] = 0.0f;
		data[counter++] = 0.0f;
		data[counter++] = 200.0f;

		data[counter++] = zColor.x;
		data[counter++] = zColor.y;
		data[counter++] = zColor.z;
		data[counter++] = zColor.w;
	}


	if (xzPlane) 
	{
		for (int i = min; i < max; i++) 
		{

		// Line 1
			// point 1
			data[counter++] = min;
			data[counter++] = 0.0f;
			data[counter++] = i;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

			// point 2
			data[counter++] = max;
			data[counter++] = 0.0f;
			data[counter++] = i;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

		// Line 2
			// point 1
			data[counter++] = i;
			data[counter++] = 0.0f;
			data[counter++] = min;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

			// point 2
			data[counter++] = i;
			data[counter++] = 0.0f;
			data[counter++] = max;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

		}
	}

	if (xyPlane)
	{
		for (int i = min; i < max; i++)
		{

			// Line 1
				// point 1
			data[counter++] = min;
			data[counter++] = i;
			data[counter++] = 0.0f;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

			// point 2
			data[counter++] = max;
			data[counter++] = i;
			data[counter++] = 0.0f;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

			// Line 2
				// point 1
			data[counter++] = i;
			data[counter++] = min;
			data[counter++] = 0.0f;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

			// point 2
			data[counter++] = i;
			data[counter++] = max;
			data[counter++] = 0.0f;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

		}
	}

	if (zyPlane)
	{
		for (int i = min; i < max; i++)
		{

			// Line 1
				// point 1
			data[counter++] = 0.0f;
			data[counter++] = i;
			data[counter++] = min;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

			// point 2
			data[counter++] = 0.0f;
			data[counter++] = i;
			data[counter++] = max;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

			// Line 2
				// point 1
			data[counter++] = 0.0f;
			data[counter++] = min;
			data[counter++] = i;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

			// point 2
			data[counter++] = 0.0f;
			data[counter++] = max;
			data[counter++] = i;

			// color
			data[counter++] = color.x;
			data[counter++] = color.y;
			data[counter++] = color.z;
			data[counter++] = color.w;

		}
	}



	m_Program = new opengl::Program("res/shaders/opengl-cookbook/common/GridCoords.shader");
	m_VBA = new opengl::VertexArray();
	m_VBO = new opengl::VertexBuffer((void*) data, byteCount);
	m_VBL = new opengl::VertexBufferLayout();

	delete[] data;

	m_VBL->Push<float>(3);
	m_VBL->Push<float>(4);

	m_VBA->AddBuffer(*m_VBO, *m_VBL);

	m_Program->Unbind();
	m_VBA->Unbind();
	m_VBO->Unbind();

}

core::Grid::~Grid()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBA;
	delete m_VBO;
}

void core::Grid::BindAndDraw(glm::mat4& m_ProjMat, glm::mat4& m_ViewMat)
{
	m_Program->Bind();

	m_Program->SetUniformMat4f("u_Projection", m_ProjMat);
	m_Program->SetUniformMat4f("u_View", m_ViewMat);

	m_VBA->Bind();

	GLCall(glDrawArrays(GL_LINES, 0, this->vertexCount));
}

void core::Grid::Unbind()
{
	
}