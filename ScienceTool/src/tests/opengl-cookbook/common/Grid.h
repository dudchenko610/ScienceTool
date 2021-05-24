#pragma once

#include "../../../core/opengl/program/Program.h"
#include "../../../core/opengl/vertex-array/VertexArray.h"
#include "../../../core/opengl/vertex-buffer/VertexBuffer.h"
#include "../../../core/opengl/vertex-buffer/layout/VertexBufferLayout.h"

//#include <../../Math/src/glm/glm.hpp>

namespace core
{
	class Grid
	{
	public:
		Grid(bool xzPlane, bool xyPlane, bool zyPlane);
		~Grid();
	public:
		void BindAndDraw(glm::mat4& m_ProjMat, glm::mat4& m_ViewMat);
		void Unbind();
	private:
		//void** m_Data;

		opengl::Program* m_Program;
		opengl::VertexArray* m_VBA;
		opengl::VertexBuffer* m_VBO;
		opengl::VertexBufferLayout* m_VBL;

		int vertexCount = 0;
	};
}



