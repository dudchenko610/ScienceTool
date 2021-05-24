#include "Example_6_4_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

/*

This shader can be dropped in and applied to any mesh without any modification to the
OpenGL side of the application. It can be useful when debugging mesh issues or when
implementing a mesh modeling program.

*/

core::Example_6_4_Window::Example_6_4_Window(const int w, const int h) : Window(w, h, "Example_6_4_Window"),

	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ViewMat(glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0)))

{
	std::cout << "Example_6_4_Window Constructor " << std::endl;

	float w2 = w / 2.0f;
	float h2 = h / 2.0f;

	this->m_ViewportMat = glm::mat4(
		glm::vec4(w2 + 0, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, h2 + 0, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
		glm::vec4(w2 + 0, h2 + 0, 0.0f, 1.0f));

}

core::Example_6_4_Window::~Example_6_4_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;

}

void core::Example_6_4_Window::Init()
{
	std::cout << "Example_6_4_Window Init" << std::endl;
	using namespace opengl;

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	float verts[] = { 
		-1.0f, -1.0f,
		 1.0f, -1.0f, 
		 1.0f,  1.0f, 
		-1.0f,  1.0f
	};


	m_Program = new Program("res/shaders/opengl-cookbook/chapter6/ex_4", "TessQuad_VS.glsl", "TessQuad_FS.glsl", "TessQuad_GS.glsl", "TessQuad_TCS.glsl", "TessQuad_TES.glsl");

	m_VBA = new VertexArray();
	m_VBO = new VertexBuffer(verts, 4 * 2 * sizeof(float));
	m_VBL = new VertexBufferLayout();


	m_VBL->Push<float>(2);

	m_VBA->AddBuffer(*m_VBO, *m_VBL);


	m_Program->Bind();
	m_Program->SetUniformMat4f("u_Projection", m_ProjMat);
	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniformMat4f("u_Model", m_ModelMat);
	m_Program->SetUniformMat4f("ViewportMatrix", m_ViewportMat);

	m_Program->SetUniform1i("Inner", 100);
	m_Program->SetUniform1i("Outer", 100);

	m_Program->SetUniform1f("LineWidth", 0.3f);

	m_Program->SetUniform4f("LineColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	m_Program->SetUniform4f("QuadColor", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	m_Program->Unbind();
	m_VBA->Unbind();


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void core::Example_6_4_Window::Loop(float dt)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_Program->Bind();
	m_VBA->Bind();


	GLCall(glDrawArrays(GL_PATCHES, 0, 4));

}

void core::Example_6_4_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_6_4_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_6_4_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_6_4_Window::OnScrollAction(double xoffset, double yoffset)
{
}
