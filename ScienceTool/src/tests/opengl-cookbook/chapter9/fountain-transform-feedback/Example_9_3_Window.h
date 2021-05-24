#pragma once

#include "../../../../core/window/Window.h"

#include "../../../../core/opengl/program/Program.h"
#include "../../../../core/opengl/vertex-array/VertexArray.h"
#include "../../../../core/opengl/vertex-buffer/VertexBuffer.h"
#include "../../../../core/opengl/vertex-buffer/layout/VertexBufferLayout.h"

#include "../../common/Grid.h"


namespace core
{
	class Example_9_3_Window : Window
	{
	public:
		Example_9_3_Window(const int w, const int h);
		~Example_9_3_Window();
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

		opengl::Program* m_Program;
		GLuint programId;

		glm::mat4 m_ProjMat;
		glm::mat4 m_ViewMat;

		glm::mat4 m_ModelMat;

		GLuint posVBO[2], velVBO[2], startTimeVBO[2], initVelVBO;
		GLuint particleVAO[2];

		GLuint feedback[2];
		GLuint drawBuf, query;

		GLuint renderSubroutineId, updateSubroutineId;

		float time = 0;

		int nParticles;
	};
}



