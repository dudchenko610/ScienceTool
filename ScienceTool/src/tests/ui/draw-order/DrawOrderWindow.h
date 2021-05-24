#pragma once

#include "../../../core/window/Window.h"

#include "../../../core/opengl/program/Program.h"
#include "../../../core/opengl/vertex-array/VertexArray.h"
#include "../../../core/opengl/vertex-buffer/VertexBuffer.h"
#include "../../../core/opengl/vertex-buffer/layout/VertexBufferLayout.h"

#include "../../opengl-cookbook/common/Grid.h"


namespace core
{
	class DrawOrderWindow : Window
	{

	public:
		DrawOrderWindow(const int w, const int h);
		~DrawOrderWindow();
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

		opengl::Program* m_RenderProg;


		glm::mat4 m_ProjMat;
		glm::mat4 m_ViewMat;
		glm::mat4 m_ModelMat;


		GLuint VAO;
		GLuint VBO;
		GLuint IBO;

		GLuint planeVBO;
		GLuint planeVAO;
		GLuint planeIBO;

		GLuint SSBO_Counters;
		GLuint SSBO_Buffer;

		unsigned int pass1Id;
		unsigned int pass2Id;

		GLuint clearVals [4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }; // 0xFFFFFFFF;

		float time;
		unsigned int count;


	};
}



