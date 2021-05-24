#pragma once

#include "../../../../core/window/Window.h"

#include "../../../../core/opengl/program/Program.h"
#include "../../../../core/opengl/vertex-array/VertexArray.h"
#include "../../../../core/opengl/vertex-buffer/VertexBuffer.h"
#include "../../../../core/opengl/vertex-buffer/layout/VertexBufferLayout.h"

#include "../../common/Grid.h"

namespace core
{
	class Example_9_2_Window : Window
	{
	public:
		Example_9_2_Window(const int w, const int h);
		~Example_9_2_Window();
	public:
		void Init();
		void Loop(float dt);
		void OnKeyAction(int key, int scancode, int action, int mods);

		void OnMouseMoveAction(double xpos, double ypos);
		void OnMouseButtonAction(int button, int action, int mods);
		void OnScrollAction(double xoffset, double yoffset);

	private:
		float randFloat();
	private:
		// order in initialization list depends on order of next declarations

		Grid* m_Grid;

		opengl::Program* m_Program;

		opengl::VertexArray* m_VBA;

		opengl::VertexBuffer* m_VBO_Pos;
		opengl::VertexBufferLayout* m_VBL_Pos;

		opengl::VertexBuffer* m_VBO_Time;
		opengl::VertexBufferLayout* m_VBL_Time;

		glm::mat4 m_ProjMat;
		glm::mat4 m_ViewMat;

		glm::mat4 m_ModelMat_Torus;
		glm::mat4 m_ModelMat_Plane;
		glm::mat4 m_ViewportMat;

		float time = 0;

		int nParticles;
	};
}




