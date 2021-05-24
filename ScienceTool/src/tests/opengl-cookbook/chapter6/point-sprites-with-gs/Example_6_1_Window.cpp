#include "Example_6_1_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

#include "stb_image/stb_image.h"

core::Example_6_1_Window::Example_6_1_Window(const int w, const int h) : Window(w, h, "Example_6_1_Window"),

	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ViewMat(glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0)))

{
	std::cout << "Example_6_1_Window Constructor " << std::endl;

}

core::Example_6_1_Window::~Example_6_1_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;

	delete m_Grid;
}

void core::Example_6_1_Window::Init()
{
	std::cout << "Example_2_1_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	this->pointsCount = 6;

	float* dataPoints = new float[pointsCount * 3] {
		-3.0f, 0.0f, 0.0f,
		4.0f, 1.0f, 0.0f,
		1.0f, 2.0f, 0.0f,
		2.0f, 1.0f, 1.0f,
		3.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
	};

	

	m_Program = new Program("res/shaders/opengl-cookbook/chapter6/ex_1", "PointSprites_VS.glsl", "PointSprites_FS.glsl", "PointSprites_GS.glsl");
	m_VBA = new VertexArray();
	m_VBO = new VertexBuffer(dataPoints, pointsCount * 3 * sizeof(float)); // size in bytes
	m_VBL = new VertexBufferLayout();

	delete[] dataPoints;

	m_VBL->Push<float>(3);

	m_VBA->AddBuffer(*m_VBO, *m_VBL);


	m_Program->Bind();
	m_Program->SetUniformMat4f("u_Projection", m_ProjMat);
	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniformMat4f("u_Model", m_ModelMat);


	int width, height, bpp;
	unsigned char* data = stbi_load("res/textures/tex1.png", &width, &height, &bpp, 4);

	// Copy file to OpenGL
	unsigned int texId;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height); // allocate memory
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (data)
	{
		stbi_image_free(data);
	}

	m_Program->SetUniform1i("SpriteTex", 0);
	m_Program->SetUniform1f("Size2", 0.1f);


	m_Program->Unbind();
	m_VBA->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void core::Example_6_1_Window::Loop(float dt)
{
	time += dt;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_Program->Bind();
	m_VBA->Bind();

	float radius = 2.5f;

	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), 0.5f, radius * glm::cos(time));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program->SetUniformMat4f("u_View", m_ViewMat);

	GLCall(glDrawArrays(GL_POINTS, 0, this->pointsCount));

	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();
}

void core::Example_6_1_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_6_1_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_6_1_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_6_1_Window::OnScrollAction(double xoffset, double yoffset)
{
}
