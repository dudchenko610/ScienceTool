#include "Example_6_2_Window.h"


#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

/*

This shader can be dropped in and applied to any mesh without any modification to the
OpenGL side of the application. It can be useful when debugging mesh issues or when
implementing a mesh modeling program.

*/
	
core::Example_6_2_Window::Example_6_2_Window(const int w, const int h) : Window(w, h, "Example_6_2_Window"),

	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ViewMat(glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0))),

	trianglesCount(0)

{
	std::cout << "Example_6_2_Window Constructor " << std::endl;

	float w2 = w / 2.0f;
	float h2 = h / 2.0f;

	this->m_ViewportMat = glm::mat4(glm::vec4(w2+0, 0.0f, 0.0f, 0.0f),
							  glm::vec4(0.0f, h2+0, 0.0f, 0.0f),
							  glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
							  glm::vec4(w2+0, h2+0, 0.0f, 1.0f));

}

core::Example_6_2_Window::~Example_6_2_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;

	delete m_Grid;
}

void core::Example_6_2_Window::Init()
{
	std::cout << "Example_6_2_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	parser::Vertices* verts = parser::obj::ObjParser::Parse("res/obj/torus_knot.objj");
	this->trianglesCount = verts->GetTrianglesCount();

	m_Program = new Program("res/shaders/opengl-cookbook/chapter6/ex_2", "Wireframe_VS.glsl", "Wireframe_FS.glsl", "Wireframe_GS.glsl");

	m_VBA = new VertexArray();
	m_VBO = new VertexBuffer(verts->GetData(), verts->GetSize());
	m_VBL = new VertexBufferLayout();

	delete verts;

	m_VBL->Push<float>(3);
	m_VBL->Push<float>(3);

	m_VBA->AddBuffer(*m_VBO, *m_VBL);


	m_Program->Bind();
	m_Program->SetUniformMat4f("u_Projection", m_ProjMat);
	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniformMat4f("u_Model", m_ModelMat);
	m_Program->SetUniformMat4f("u_Viewport", m_ViewportMat);


	struct LightInfo {
		glm::vec3 position;
		glm::vec3 Intensity;
	};

	struct MaterialInfo {
		glm::vec3 Ka;		 // ambient reflectivity
		glm::vec3 Kd;		 // diffuse reflectivity
		glm::vec3 Ks;		 // specular reflectivity
		float Shininess; // specular shininess factor
	};

	float radius = 3.0f;
	/*
	m_Program->SetUniform4f("lights[0].Position", glm::vec4(-radius, 0.0f, 0.0f, 0.0f));
	m_Program->SetUniform3f("lights[0].Intensity", glm::vec3(0.0f, 0.0f, 1.0f));*/


	m_Program->SetUniform3f("lights[0].Position", glm::vec3(-radius, 0.0f, 0.0f));
	m_Program->SetUniform3f("lights[0].Intensity", glm::vec3(0.0f, 0.0f, 1.0f));

	m_Program->SetUniform3f("lights[1].Position", glm::vec3(radius, 0.0f, 0.0f));
	m_Program->SetUniform3f("lights[1].Intensity", glm::vec3(1.0f, 0.0f, 0.0f));

	m_Program->SetUniform3f("lights[2].Position", glm::vec3(0.0, 0.0f, radius));
	m_Program->SetUniform3f("lights[2].Intensity", glm::vec3(0.0f, 0.0f, 0.0f));

	m_Program->SetUniform3f("lights[3].Position", glm::vec3(0.0, 0.0f, -radius));
	m_Program->SetUniform3f("lights[3].Intensity", glm::vec3(0.0f, 0.0f, 0.0f));

	m_Program->SetUniform3f("lights[4].Position", glm::vec3(0.0, radius, 0.0f));
	m_Program->SetUniform3f("lights[4].Intensity", glm::vec3(0.0f, 0.0f, 0.0f));

	MaterialInfo materialInfo;
	materialInfo.Ka = glm::vec3(0.1f, 0.1f, 0.1f);
	materialInfo.Kd = glm::vec3(0.3f, 0.3f, 0.3f);
	materialInfo.Ks = glm::vec3(0.9f, 0.9f, 0.9f);
	materialInfo.Shininess = 8;

	m_Program->SetUniform1f("Shininess", materialInfo.Shininess);
	m_Program->SetUniform3f("Ka", materialInfo.Ka);
	m_Program->SetUniform3f("Kd", materialInfo.Kd);
	m_Program->SetUniform3f("Ks", materialInfo.Ks);

	struct LineInfo
	{
		float Width;
		glm::vec4 Color;
	};

	LineInfo line;
	line.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0);
	line.Width = 0.75f;





	m_Program->Unbind();
	m_VBA->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void core::Example_6_2_Window::Loop(float dt)
{
	time += dt;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_Program->Bind();
	m_VBA->Bind();

	float radius = 2.5f;

	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), radius * glm::cos(time), radius * glm::cos(time));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniform3f("u_CameraPos", cameraPos);

	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCount * 3));

	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();
}

void core::Example_6_2_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_6_2_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_6_2_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_6_2_Window::OnScrollAction(double xoffset, double yoffset)
{
}
