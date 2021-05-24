#include "Example_2_2_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"




core::Example_2_2_Window::Example_2_2_Window(const int w, const int h, const char* l) : Window(w, h, l),

	//	m_ProjMat(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -0.1f, 10.0f)),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ViewMat(glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0))),

	trianglesCount(0)

{
	std::cout << "Example_2_2_Window Constructor " << std::endl;

}

core::Example_2_2_Window::~Example_2_2_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;

	delete m_Grid;
}

void core::Example_2_2_Window::Init()
{
	std::cout << "Example_2_1_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	parser::Vertices* verts = parser::obj::ObjParser::Parse("res/obj/torus_knot.objj");
	this->trianglesCount = verts->GetTrianglesCount();

	//m_Program = new Program("res/shaders/opengl-cookbook/chapter2/ex_2/ADS_Shading.shader");
	//m_Program = new Program("res/shaders/opengl-cookbook/chapter2/ex_2/TwoSided_Shading.shader");
	m_Program = new Program("res/shaders/opengl-cookbook/chapter2/ex_2/Flat_Shading.shader");
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


	struct LightInfo {
		glm::vec3 position; // light position
		glm::vec3 La;       // ambient light intensity
		glm::vec3 Ld;       // diffuse light intensity
		glm::vec3 Ls;       // specular light intensity;
	};

	struct MaterialInfo {
		glm::vec3 Ka;		 // ambient reflectivity
		glm::vec3 Kd;		 // diffuse reflectivity
		glm::vec3 Ks;		 // specular reflectivity
		float Shininess; // specular shininess factor
	}; 

	glm::vec3 LightPosition = glm::vec3(2.5f, 0.0f, 0.0f);

	LightInfo lightInfo;
	lightInfo.position = LightPosition;
	lightInfo.La = glm::vec3(0.1f, 0.1f, 0.3f);
	lightInfo.Ld = glm::vec3(0.1f, 0.3f, 0.5f);
	lightInfo.Ls = glm::vec3(0.1f, 0.2f, 0.7f);

	
	m_Program->SetUniform3f("Light.Position", lightInfo.position);
	m_Program->SetUniform3f("Light.La", lightInfo.La);
	m_Program->SetUniform3f("Light.Ld", lightInfo.Ld);
	m_Program->SetUniform3f("Light.Ls", lightInfo.Ls);


	MaterialInfo materialInfo;
	materialInfo.Ka = glm::vec3(0.1f, 0.1f, 0.1f);
	materialInfo.Kd = glm::vec3(0.1f, 0.3f, 0.6f);
	materialInfo.Ks = glm::vec3(0.1f, 0.4f, 0.9f);
	materialInfo.Shininess = 6;

	m_Program->SetUniform1f("Material.Shininess", materialInfo.Shininess);
	m_Program->SetUniform3f("Material.Ka", materialInfo.Ka);
	m_Program->SetUniform3f("Material.Kd", materialInfo.Kd);
	m_Program->SetUniform3f("Material.Ks", materialInfo.Ks);

	

	m_Program->Unbind();
	m_VBA->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void core::Example_2_2_Window::Loop(float dt)
{
	time += dt;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_Program->Bind();
	m_VBA->Bind();

	float radius = 2.5f;

	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), 0.5f, radius * glm::cos(time));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniform3f("u_CameraPos", cameraPos);

	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCount * 3));

	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();
}

void core::Example_2_2_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_2_2_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_2_2_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_2_2_Window::OnScrollAction(double xoffset, double yoffset)
{
}
