#pragma once
#include <mutex>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace core 
{
	class Window
	{
	public:
		Window(const int width, const int height, const char* label);
		~Window();
	public:
		virtual void Init() = 0;
		virtual void Loop(float dt) = 0;
	public:

		virtual void OnMouseMoveAction(double xpos, double ypos) = 0;
		virtual void OnMouseButtonAction(int button, int action, int mods) = 0;
		virtual void OnScrollAction(double xoffset, double yoffset) = 0;


		virtual void OnKeyAction(int key, int scancode, int action, int mods) = 0;
	public:
		void Run();
	private:
		static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void ErrorCallback(int error, const char* description);
	protected:
		void GetCursorPosition(double* x, double* y);
	private:
		std::mutex mtx;
	protected:

		GLFWwindow* glfwWindow;

		const int width;
		const int height;
		const char* label;

	};
}



