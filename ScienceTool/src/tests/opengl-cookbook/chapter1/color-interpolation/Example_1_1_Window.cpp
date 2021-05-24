#include "Example_1_1_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

core::Example_1_1_Window::Example_1_1_Window(const int w, const int h, const char* l) : Window(w, h, l)
{

}

core::Example_1_1_Window::~Example_1_1_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;
}


void core::Example_1_1_Window::Init()
{
	using namespace opengl;
	std::cout << "Example_1_1_Window Init " << std::endl;


	float* data = new float[18]
	{
		-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f
	};


	m_Program = new Program("res/shaders/opengl-cookbook/chapter1/color-interpolation/ColorInterpolation.shader");
	m_VBA = new VertexArray();
	m_VBO = new VertexBuffer(data, 3 * (2 + 4) * sizeof(float));
	m_VBL = new VertexBufferLayout();

	m_VBL->Push<float>(2);
	m_VBL->Push<float>(4);

	m_VBA->AddBuffer(*m_VBO, *m_VBL);

	m_Program->Unbind();
	m_VBA->Unbind();
}

void core::Example_1_1_Window::Loop(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_Program->Bind();
	m_VBA->Bind();

	GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));
}

void core::Example_1_1_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}


}

void core::Example_1_1_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_1_1_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_1_1_Window::OnScrollAction(double xoffset, double yoffset)
{
}