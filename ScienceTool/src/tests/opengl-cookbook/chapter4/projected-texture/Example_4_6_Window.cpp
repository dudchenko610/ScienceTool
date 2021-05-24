#include "Example_4_6_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

#include "stb_image/stb_image.h"

core::Example_4_6_Window::Example_4_6_Window(const int w, const int h, const char* l) : Window(w, h, l),

	//	m_ProjMat(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -0.1f, 10.0f)),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 200.0f)),
	m_ViewMat(glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0))),

	trianglesCount(0)

{
	std::cout << "Example_4_6_Window Constructor " << std::endl;

}

core::Example_4_6_Window::~Example_4_6_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;

	delete m_Grid;

	delete m_VBA_Cube;
	delete m_VBO_Cube;
	delete m_VBL_Cube;

}

void core::Example_4_6_Window::Init()
{
	std::cout << "Example_4_6_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	m_Program = new Program("res/shaders/opengl-cookbook/chapter4/ex_6/ProjectedTexture.shader");

	parser::Vertices* vertsCube = parser::obj::ObjParser::Parse("res/obj/plane.objj");
	this->trianglesCountCube = vertsCube->GetTrianglesCount();

	m_VBA_Cube = new VertexArray();
	m_VBO_Cube = new VertexBuffer(vertsCube->GetData(), vertsCube->GetSize());
	m_VBL_Cube = new VertexBufferLayout();

	delete vertsCube;

	m_VBL_Cube->Push<float>(3);
	m_VBL_Cube->Push<float>(3);


	m_VBA_Cube->AddBuffer(*m_VBO_Cube, *m_VBL_Cube);

	// object

	parser::Vertices* verts = parser::obj::ObjParser::Parse("res/obj/torus_knot.objj");
	this->trianglesCount = verts->GetTrianglesCount();

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

	m_Program->SetUniform3f("Light.Position", glm::vec3(-radius, 1.0f, 0.0f));
	m_Program->SetUniform3f("Light.Intensity", glm::vec3(0.2f, 0.3f, 0.7f));


	MaterialInfo materialInfo;
	materialInfo.Ka = glm::vec3(0.2f, 0.2f, 0.2f);
	materialInfo.Kd = glm::vec3(0.3f, 0.3f, 0.3f);
	materialInfo.Ks = glm::vec3(0.8f, 0.8f, 0.7f);
	materialInfo.Shininess = 6;

	m_Program->SetUniform1f("Material.Shininess", materialInfo.Shininess);
	m_Program->SetUniform3f("Material.Ka", materialInfo.Ka);
	m_Program->SetUniform3f("Material.Kd", materialInfo.Kd);
	m_Program->SetUniform3f("Material.Ks", materialInfo.Ks);


	// setup "texture projector"
	glm::vec3 projPos = glm::vec3( 2.0f,  5.0f, 5.0f);
	glm::vec3 projAt  = glm::vec3( 0.0f,  0.0f, 0.0f);
	glm::vec3 projUp  = glm::vec3( 0.0f,  1.0f, 0.0f);

	glm::mat4 projView = glm::lookAt(projPos, projAt, projUp);
	glm::mat4 projProj = glm::perspective(50.0f, 1.0f, 0.2f, 1000.0f);

	glm::mat4 projScaleTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

	glm::mat4 m = projScaleTrans * projProj * projView;

	m_Program->SetUniformMat4f("ProjectorMatrix", m);

	// Upload texture

	int width, height, bpp;
	unsigned char* data = stbi_load("res/textures/tex1.png", &width, &height, &bpp, 4);


	glActiveTexture(GL_TEXTURE0);

	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height); // allocate memory
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	if (data)
	{
		stbi_image_free(data);
	}

	// Typical cube map settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


	m_Program->Unbind();
	m_VBA->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void core::Example_4_6_Window::Loop(float dt)
{
	time += dt;

	//	std::cout << "FPS" << (1 / dt) << std::endl;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_Program->Bind();

	float radius = 2.5f;
	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), 2.5f, radius * glm::cos(time));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // eye lookAt up-vector

	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniform3f("u_CameraPos", cameraPos);

	// draw plane
	m_VBA_Cube->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountCube * 3));

	// draw torus knot
	m_VBA->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCount * 3));



	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();
}

void core::Example_4_6_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_4_6_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_4_6_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_4_6_Window::OnScrollAction(double xoffset, double yoffset)
{
}
