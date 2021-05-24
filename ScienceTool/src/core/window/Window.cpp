#include "Window.h"

#include <iostream>
#include <chrono> 

#include "../application/Application.h"

void GLAPIENTRY  debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	// Convert GLenum parameters to strings

//	std::cout << "Source = " << source << std::endl;
//	std::cout << "Message = " << message << std::endl;

	//printf("%s:%s[%s](%d): %s\n", sourceStr, typeStr, severityStr, id, message);
}

core::Window::Window(const int w, const int h, const char* l) :
	width(w), height(h), label(l), glfwWindow(nullptr)
{
	std::cout << "Window Constructor " << std::endl;

}

core::Window::~Window()
{
	std::cout << "Window Destructor " << std::endl;
}

void core::Window::Run()
{

	mtx.lock();

	glfwSetErrorCallback(&core::Window::ErrorCallback);

	glfwWindow = glfwCreateWindow(width, height, label, NULL, NULL);

	if (!glfwWindow)
	{
		mtx.unlock();

		core::Event* event = new Event();
		event->code = 0;
		event->data = this;

		core::Application::Get().looper.PushEvent(event);

		return;
	}

	// Error
	glfwSetWindowUserPointer(glfwWindow, this);

	// Key
	glfwSetKeyCallback(glfwWindow, &core::Window::KeyCallback);

	// Mouse
	glfwSetCursorPosCallback(glfwWindow, &core::Window::MouseMoveCallback);
	glfwSetMouseButtonCallback(glfwWindow, &core::Window::MouseButtonCallback);
	glfwSetScrollCallback(glfwWindow, &core::Window::ScrollCallback);

	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(1);

	mtx.unlock();

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error while GLEW init!" << std::endl;
	}

	const unsigned char* renderer    = glGetString(GL_RENDERER);
	const unsigned char* vendor      = glGetString(GL_VENDOR);
	const unsigned char* version     = glGetString(GL_VERSION);
	const unsigned char* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	//std::cout << std::endl;


	printf("GL Vendor            : %s\n", vendor);
	printf("GL Renderer          : %s\n", renderer);
	printf("GL Version (string)  : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version         : %s\n", glslVersion);

	/*

	GLint nExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
	for (int i = 0; i < nExtensions; i++)
		printf("%s\n", glGetStringi(GL_EXTENSIONS, i));


	*/

	//std::cout << glGetString(GL_VERSION) << std::endl;

	glDebugMessageCallback(debugCallback, NULL);

	this->Init();

	std::chrono::time_point<std::chrono::steady_clock> startPoint = std::chrono::high_resolution_clock::now();

	std::chrono::microseconds delay;
	float dt;

	while (!glfwWindowShouldClose(glfwWindow))
	{

		delay = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startPoint);
		startPoint = std::chrono::high_resolution_clock::now();

		dt = delay.count() / 1000000.0f;
		this->Loop(dt);

		glfwSwapBuffers(glfwWindow);
		glfwPollEvents();


	}

	mtx.lock();
	glfwDestroyWindow(glfwWindow);
	mtx.unlock();


	core::Event* event = new Event();
	event->code = 0;
	event->data = this;

	core::Application::Get().looper.PushEvent(event);
}

void core::Window::ErrorCallback(int error, const char* description)
{
	std::cout << "code = " << error << " description = " << description << std::endl;
}

void core::Window::GetCursorPosition(double* xpos, double* ypos)
{
	glfwGetCursorPos(glfwWindow, xpos, ypos);
}

void core::Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* myWnd = (Window*) glfwGetWindowUserPointer(window);
	myWnd->OnKeyAction(key, scancode, action, mods);
}

void core::Window::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	Window* myWnd = (Window*)glfwGetWindowUserPointer(window);
	myWnd->OnMouseMoveAction(xpos, ypos);
}

void core::Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Window* myWnd = (Window*)glfwGetWindowUserPointer(window);
	myWnd->OnMouseButtonAction(button, action, mods);
}

void core::Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Window* myWnd = (Window*)glfwGetWindowUserPointer(window);
	myWnd->OnScrollAction(xoffset, yoffset);
}






