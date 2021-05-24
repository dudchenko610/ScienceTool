#pragma once

#include "../../../../core/window/Window.h"

#include "../../../../core/opengl/program/Program.h"
#include "../../../../core/opengl/vertex-array/VertexArray.h"
#include "../../../../core/opengl/vertex-buffer/VertexBuffer.h"
#include "../../../../core/opengl/vertex-buffer/layout/VertexBufferLayout.h"

#include "../../common/Grid.h"


namespace core
{
	class Example_5_6_Window : Window
	{
	public:
		Example_5_6_Window(const int w, const int h, const char* l);
		~Example_5_6_Window();
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

		opengl::VertexArray* m_VBA_Torus;
		opengl::VertexBuffer* m_VBO_Torus;
		opengl::VertexBufferLayout* m_VBL_Torus;

		opengl::VertexArray* m_VBA_Sphere;
		opengl::VertexBuffer* m_VBO_Sphere;
		opengl::VertexBufferLayout* m_VBL_Sphere;

		opengl::VertexArray* m_VBA_Plane;
		opengl::VertexBuffer* m_VBO_Plane;
		opengl::VertexBufferLayout* m_VBL_Plane;

		glm::mat4 m_ProjMat;
		glm::mat4 m_ModelMat;

		int trianglesCountTorus;
		int trianglesCountSphere;
		int trianglesCountPlane;

		float time = 0;

		unsigned int _pass1_SubroutineIndex;
		unsigned int _pass2_SubroutineIndex;

		unsigned int counterBufferId;
		unsigned int headPtrTexId;
		unsigned int clearBufId;

		bool isCorrectOrder = false;

		bool b = true;
	};
}

