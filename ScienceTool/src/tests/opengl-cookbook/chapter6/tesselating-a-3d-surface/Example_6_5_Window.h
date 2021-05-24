#pragma once


#include "../../../../core/window/Window.h"

#include "../../../../core/opengl/program/Program.h"
#include "../../../../core/opengl/vertex-array/VertexArray.h"
#include "../../../../core/opengl/vertex-buffer/VertexBuffer.h"
#include "../../../../core/opengl/vertex-buffer/layout/VertexBufferLayout.h"

#include "../../common/Grid.h"

#include <vector>

namespace core
{
	class Example_6_5_Window : Window
	{
	public:
		Example_6_5_Window(const int w, const int h);
		~Example_6_5_Window();
	public:
		void Init();
		void Loop(float dt);
		void OnKeyAction(int key, int scancode, int action, int mods);


		void OnMouseMoveAction(double xpos, double ypos);
		void OnMouseButtonAction(int button, int action, int mods);
		void OnScrollAction(double xoffset, double yoffset);

	private:
		void generatePatches(std::vector<float>& pts);
		void buildPatchReflect(int patchNum, std::vector<float>& p, int& index, bool reflectX, bool reflectY);
		void buildPatch(glm::vec3 patch[][4], std::vector<float>& pts, int& index, glm::mat3 reflect);
		void getPatch(int patchNum, glm::vec3 patch[][4], bool reverseV);
	private:
		// order in initialization list depends on order of next declarations

		Grid* m_Grid;

		opengl::Program* m_Program;
		opengl::VertexArray* m_VBA;
		opengl::VertexBuffer* m_VBO;
		opengl::VertexBufferLayout* m_VBL;

		glm::mat4 m_ProjMat;
		glm::mat4 m_ViewMat;
		glm::mat4 m_ModelMat;
		glm::mat4 m_ViewportMat;

		float time = 0;

	};

}


