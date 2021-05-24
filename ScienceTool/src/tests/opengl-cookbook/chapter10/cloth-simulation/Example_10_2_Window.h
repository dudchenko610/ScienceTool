#pragma once

#include "../../../../core/window/Window.h"

#include "../../../../core/opengl/program/Program.h"
#include "../../../../core/opengl/vertex-array/VertexArray.h"
#include "../../../../core/opengl/vertex-buffer/VertexBuffer.h"
#include "../../../../core/opengl/vertex-buffer/layout/VertexBufferLayout.h"

#include "../../common/Grid.h"


namespace core
{
	class Example_10_2_Window : Window
	{
	public:
		Example_10_2_Window(const int w, const int h);
		~Example_10_2_Window();
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
		opengl::Program* m_ComputeNormProg;


		glm::mat4 m_ProjMat;
		glm::mat4 m_ViewMat;
		glm::mat4 m_ModelMat;


		GLuint clothVAO;
		GLuint numElements;

		glm::ivec2 nParticles;  // Number of particles in each dimension
		glm::vec2 clothSize;    // Size of cloth in x and y

		float time, deltaT, speed;
		GLuint readBuf;
		GLuint posBufsVBO[2], velBufsVBO[2];
		GLuint normBufVBO, elBufVBO, tcBufVBO;
	};
}

