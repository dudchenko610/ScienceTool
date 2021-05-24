#include "TestWindow.h"

#include <iostream>
#include "../../core/application/Application.h"
#include "../../core/opengl/core/error/ErrorChecker.h"
#include "../opengl-cookbook/chapter1/color-interpolation/Example_1_1_Window.h"
#include "../opengl-cookbook/chapter2/diffuse-per-vertex-shading-with-one-light-source/Example_2_1_Window.h"


/*void GLAPIENTRY  debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	// Convert GLenum parameters to strings

	std::cout << "Source = " << source << std::endl;
	std::cout << "Message = " << message << std::endl;

	//printf("%s:%s[%s](%d): %s\n", sourceStr, typeStr, severityStr, id, message);
}*/

core::TestWindow::TestWindow(const int w, const int h, const char* l) : Window(w, h, l)
{
	
}

core::TestWindow::~TestWindow()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;
}

void core::TestWindow::Init()
{
	using namespace opengl;
	std::cout << "TestWindow Init " << std::endl;

	//glDebugMessageCallback(debugCallback, NULL); // NULL === const void* userParam in callback - custom user data
	/*glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
		GL_DONT_CARE, 0, NULL, GL_TRUE);*/

	
	float* data = new float[6]
	{
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};
	

	m_Program = new Program("res/shaders/test1/Shader.shader");
	m_VBA = new VertexArray();
	m_VBO = new VertexBuffer(data, 3 * 2 * sizeof(float));
	m_VBL = new VertexBufferLayout();

	m_VBL->Push<float>(2);
	m_VBA->AddBuffer(*m_VBO, *m_VBL);


	m_Program->Bind();
	m_Program->SetUniform4f("u_Color", 1.0f, 1.0f, 0.0f, 1.0f);

	m_Program->Unbind();
	m_VBA->Unbind();
}

void core::TestWindow::Loop(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_Program->Bind();
	m_VBA->Bind();

	GLCall(glDrawArrays(GL_TRIANGLES,0, 3));
}

void core::TestWindow::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		Window* baseWnd = (Window*) new core::TestWindow(640, 400, "TestWindow");

		core::Event* event = new Event();
		event->code = 1;
		event->data = baseWnd;

		core::Application::Get().GetLooper().PushEvent(event);

	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		Window* baseWnd = (Window*) new core::Example_1_1_Window(640, 400, "Example_1_1_Window");

		core::Event* event = new Event();
		event->code = 1;
		event->data = baseWnd;

		core::Application::Get().GetLooper().PushEvent(event);
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		Window* baseWnd = (Window*) new core::Example_2_1_Window(640, 400, "Example_2_1_Window");

		core::Event* event = new Event();
		event->code = 1;
		event->data = baseWnd;

		core::Application::Get().GetLooper().PushEvent(event);
	}

	
}

void core::TestWindow::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::TestWindow::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::TestWindow::OnScrollAction(double xoffset, double yoffset)
{
}
