#pragma once

#include "../../../../core/window/Window.h"

#include "../../../../core/opengl/program/Program.h"
#include "../../../../core/opengl/vertex-array/VertexArray.h"
#include "../../../../core/opengl/vertex-buffer/VertexBuffer.h"
#include "../../../../core/opengl/vertex-buffer/layout/VertexBufferLayout.h"

#include "../../common/Grid.h"


namespace core
{
	class Example_10_1_Window : Window
	{
	public:
		Example_10_1_Window(const int w, const int h);
		~Example_10_1_Window();
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
		opengl::Program* m_ComputeProg;


		glm::mat4 m_ProjMat;
		glm::mat4 m_ViewMat;
		glm::mat4 m_ModelMat;


		glm::ivec3 nParticles;
		GLuint totalParticles;

		float time, deltaT, speed, angle;
		GLuint particlesVAO;
		GLuint bhVAO, bhBufVBO;  // black hole VAO and buffer
		glm::vec4 bh1, bh2;

	};
}

