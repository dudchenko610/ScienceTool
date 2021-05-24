#include "Example_2_1_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"




core::Example_2_1_Window::Example_2_1_Window(const int w, const int h, const char* l) : Window(w, h, l),
	
//	m_ProjMat(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -0.1f, 10.0f)),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ViewMat(glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0))),

	trianglesCount(0)

{
	std::cout << "Example_2_1_Window Constructor " << std::endl;

}

core::Example_2_1_Window::~Example_2_1_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;

	delete m_Grid;
}

void core::Example_2_1_Window::Init()
{
	std::cout << "Example_2_1_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, true, true);
	
	parser::Vertices* verts = parser::obj::ObjParser::Parse("res/obj/cube.objj");
	this->trianglesCount = verts->GetTrianglesCount();

	m_Program = new Program("res/shaders/opengl-cookbook/chapter2/ex_1/DiffuseShading.shader");
	m_VBA = new VertexArray();
	m_VBO = new VertexBuffer(verts->GetData(), verts->GetSize());
	m_VBL = new VertexBufferLayout();

	delete verts;

	m_VBL->Push<float>(3);
	m_VBL->Push<float>(3);

	m_VBA->AddBuffer(*m_VBO, *m_VBL);

	glm::vec3 Kd = glm::vec3(1.0f, 0.5f, 0.0f);
	glm::vec3 Ld = glm::vec3(0.5f, 1.0f, 0.0f);

	glm::vec3 LightPosition = glm::vec3(-1.0f, 0.0f, -1.0f);

	m_Program->Bind();
	m_Program->SetUniformMat4f("u_Projection", m_ProjMat);
	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniformMat4f("u_Model", m_ModelMat);
			 
	m_Program->SetUniform3f("Kd", Kd);
	m_Program->SetUniform3f("Ld", Ld);
	m_Program->SetUniform3f("LightPosition", LightPosition);

	m_Program->Unbind();
	m_VBA->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void core::Example_2_1_Window::Loop(float dt)
{
	time += dt;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_Program->Bind();
	m_VBA->Bind();

	float radius = 1.5f;

	m_ViewMat = glm::lookAt(glm::vec3(radius * glm::sin(time), 0.5f, radius * glm::cos(time)), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCount * 3));

	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();
}

void core::Example_2_1_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_2_1_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_2_1_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_2_1_Window::OnScrollAction(double xoffset, double yoffset)
{
}
