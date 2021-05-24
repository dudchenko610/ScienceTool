#include "Example_9_1_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"


core::Example_9_1_Window::Example_9_1_Window(const int w, const int h) : Window(w, h, "Example_9_1_Window"),

	//	m_ProjMat(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -0.1f, 10.0f)),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),

	lightInfo(
		{
			glm::vec3(0.0f, 4.0f, 0.0f), // light position
			glm::vec3(0.1f, 0.1f, 0.3f),
			glm::vec3(0.1f, 0.3f, 0.5f),
			glm::vec3(0.1f, 0.2f, 0.7f)
		}
	),
	materialInfo(
		{
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(0.1f, 0.3f, 0.6f),
			glm::vec3(0.1f, 0.4f, 0.9f),
			6
		}
	)


{
	std::cout << "Example_9_1_Window Constructor " << std::endl;

}

core::Example_9_1_Window::~Example_9_1_Window()
{
	delete m_Program;

	delete m_VBA_Torus;
	delete m_VBO_Torus;
	delete m_VBL_Torus;


	delete m_Grid;
}

void core::Example_9_1_Window::Init()
{
	std::cout << "Example_9_1_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	m_Program = new Program("res/shaders/opengl-cookbook/chapter9/ex_1", "AnimatedSurface_VS.glsl", "AnimatedSurface_FS.glsl");

	{
		parser::Vertices* verts_Torus = parser::obj::ObjParser::Parse("res/obj/tess_plane_2.objj");
		this->trianglesCountTorus = verts_Torus->GetTrianglesCount();

		m_VBA_Torus = new VertexArray();
		m_VBO_Torus = new VertexBuffer(verts_Torus->GetData(), verts_Torus->GetSize());
		m_VBL_Torus = new VertexBufferLayout();

		delete verts_Torus;

		m_VBL_Torus->Push<float>(3);
		m_VBL_Torus->Push<float>(3);


		m_VBA_Torus->AddBuffer(*m_VBO_Torus, *m_VBL_Torus);

	}



	m_Program->Bind();

	m_Program->SetUniformMat4f("u_Projection", this->m_ProjMat);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Program->SetUniformMat4f("u_Model", model);

	m_Program->SetUniform1f("K", 8); // wavenumber
	m_Program->SetUniform1f("Velocity", 0.6f);
	m_Program->SetUniform1f("Amp", 0.04f);


	m_Program->SetUniform3f("Light.Position", this->lightInfo.position);
	m_Program->SetUniform3f("Light.La", this->lightInfo.La);
	m_Program->SetUniform3f("Light.Ld", this->lightInfo.Ld);
	m_Program->SetUniform3f("Light.Ls", this->lightInfo.Ls);

	m_Program->SetUniform1f("Material.Shininess", this->materialInfo.Shininess);
	m_Program->SetUniform3f("Material.Ka", this->materialInfo.Ka);
	m_Program->SetUniform3f("Material.Kd", this->materialInfo.Kd);
	m_Program->SetUniform3f("Material.Ks", this->materialInfo.Ks);


	m_Program->Unbind();
	m_VBA_Torus->Unbind();

	glViewport(0, 0, this->width, this->height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


}


void core::Example_9_1_Window::Loop(float dt)
{
	time += dt;

	m_Program->Bind();

	m_Program->SetUniform1f("Time", time);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float radius = 3.0;
	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time * 0.4f), 1.0f, radius * glm::cos(time * 0.4f));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniform3f("u_CameraPos", cameraPos);

	

	m_VBA_Torus->Bind();
	
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountTorus * 3));

	/*
	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();*/
}

void core::Example_9_1_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_9_1_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_9_1_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_9_1_Window::OnScrollAction(double xoffset, double yoffset)
{
}
