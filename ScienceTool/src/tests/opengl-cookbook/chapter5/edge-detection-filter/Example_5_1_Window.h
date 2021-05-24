#pragma once

#include "../../../../core/window/Window.h"

#include "../../../../core/opengl/program/Program.h"
#include "../../../../core/opengl/vertex-array/VertexArray.h"
#include "../../../../core/opengl/vertex-buffer/VertexBuffer.h"
#include "../../../../core/opengl/vertex-buffer/layout/VertexBufferLayout.h"

#include "../../common/Grid.h"

namespace core
{
	class Example_5_1_Window : Window
	{
	public:
		Example_5_1_Window(const int w, const int h, const char* l);
		~Example_5_1_Window();
	public:
		void Init();
		void Loop(float dt);
		void OnKeyAction(int key, int scancode, int action, int mods);

		void OnMouseMoveAction(double xpos, double ypos);
		void OnMouseButtonAction(int button, int action, int mods);
		void OnScrollAction(double xoffset, double yoffset);

	private:
		// order in initialization list depends on order of next declarations

		Grid* m_Grid;

		opengl::Program* m_Program_Obj;
		opengl::VertexArray* m_VBA_Obj;
		opengl::VertexBuffer* m_VBO_Obj;
		opengl::VertexBufferLayout* m_VBL_Obj;

		opengl::Program* m_Program_Plane;
		opengl::VertexArray* m_VBA_Plane;
		opengl::VertexBuffer* m_VBO_Plane;
		opengl::VertexBufferLayout* m_VBL_Plane;

		glm::mat4 m_ProjMat;
		glm::mat4 m_ModelMat;

		int trianglesCountObj;
		int trianglesCountPlane;

		float time = 0;

		unsigned int fboHandle; // The handle to framebuffer object

		unsigned int pass1SubroutineIndex;
		unsigned int pass2SubroutineIndex;
	};
}




