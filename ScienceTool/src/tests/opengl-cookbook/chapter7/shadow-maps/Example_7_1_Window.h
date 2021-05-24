#pragma once

#include "../../../../core/window/Window.h"

#include "../../../../core/opengl/program/Program.h"
#include "../../../../core/opengl/vertex-array/VertexArray.h"
#include "../../../../core/opengl/vertex-buffer/VertexBuffer.h"
#include "../../../../core/opengl/vertex-buffer/layout/VertexBufferLayout.h"

#include "../../common/Grid.h"

namespace core
{
	class Example_7_1_Window : Window
	{
	public:
		Example_7_1_Window(const int w, const int h);
		~Example_7_1_Window();
	public:
		void Init();
		void Loop(float dt);
		void OnKeyAction(int key, int scancode, int action, int mods);

		void OnMouseMoveAction(double xpos, double ypos);
		void OnMouseButtonAction(int button, int action, int mods);
		void OnScrollAction(double xoffset, double yoffset);

	private:
		void SetupFBOWithDepthBufferOnly();
	private:
		// order in initialization list depends on order of next declarations

		Grid* m_Grid;

		opengl::Program* m_Program;

		opengl::VertexArray* m_VBA_Torus;
		opengl::VertexBuffer* m_VBO_Torus;
		opengl::VertexBufferLayout* m_VBL_Torus;

		opengl::VertexArray* m_VBA_Plane;
		opengl::VertexBuffer* m_VBO_Plane;
		opengl::VertexBufferLayout* m_VBL_Plane;

		glm::mat4 m_ProjMat;
		glm::mat4 m_ViewMat;

		glm::mat4 m_ModelMat_Torus;
		glm::mat4 m_ModelMat_Plane;
		glm::mat4 m_ViewportMat;

		glm::mat4 m_ProjMat_Light;
		glm::mat4 m_ViewMat_Light;
		glm::mat4 m_BPV_Light;

		int trianglesCountTorus;
		int trianglesCountPlane;

		float time = 0;

		unsigned int depthTexId;
		unsigned int shadowFBOId;

		unsigned int shadowMapWidth;
		unsigned int shadowMapHeight;

		unsigned int pass1SubroutineIndex;
		unsigned int pass2SubroutineIndex;

		struct LightInfo {
			glm::vec3 position; // light position
			glm::vec3 La;       // ambient light intensity
			glm::vec3 Ld;       // diffuse light intensity
			glm::vec3 Ls;       // specular light intensity;
		} lightInfo;

		struct MaterialInfo {
			glm::vec3 Ka;		 // ambient reflectivity
			glm::vec3 Kd;		 // diffuse reflectivity
			glm::vec3 Ks;		 // specular reflectivity
			float Shininess; // specular shininess factor
		} materialInfo;

	};
}


