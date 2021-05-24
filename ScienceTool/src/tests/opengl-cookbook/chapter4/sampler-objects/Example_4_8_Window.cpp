#include "Example_4_8_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

#include "stb_image/stb_image.h"

core::Example_4_8_Window::Example_4_8_Window(const int w, const int h, const char* l) : Window(w, h, l),

//	m_ProjMat(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -0.1f, 10.0f)),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ViewMat(glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0))),

	trianglesCount(0)

{
	std::cout << "Example_4_8_Window Constructor " << std::endl;

}

core::Example_4_8_Window::~Example_4_8_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;

	delete m_Grid;
}

void core::Example_4_8_Window::Init()
{
	std::cout << "Example_4_8_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	parser::Vertices* verts = parser::obj::ObjParser::Parse_P_N_T("res/obj/cube_uv.objj");
	this->trianglesCount = verts->GetTrianglesCount();

	m_Program = new Program("res/shaders/opengl-cookbook/chapter4/ex_8/SamplerObjects.shader");
	m_VBA = new VertexArray();
	m_VBO = new VertexBuffer(verts->GetData(), verts->GetSize());
	m_VBL = new VertexBufferLayout();

	delete verts;

	m_VBL->Push<float>(3);
	m_VBL->Push<float>(3);
	m_VBL->Push<float>(2);

	m_VBA->AddBuffer(*m_VBO, *m_VBL);


	m_Program->Bind();
	m_Program->SetUniformMat4f("u_Projection", m_ProjMat);
	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniformMat4f("u_Model", m_ModelMat);


	// texture

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

	m_Program->SetUniform3f("Light.position", glm::vec3(0.0f, 0.0f, -radius));
	m_Program->SetUniform3f("Light.intensity", glm::vec3(0.2f, 0.3f, 0.6f));


	MaterialInfo materialInfo;
	materialInfo.Ka = glm::vec3(0.4f, 0.4f, 0.4f);
	materialInfo.Kd = glm::vec3(0.3f, 0.4f, 0.4f);
	materialInfo.Ks = glm::vec3(0.5f, 0.5f, 0.7f);
	materialInfo.Shininess = 8;

	m_Program->SetUniform1f("Shininess", materialInfo.Shininess);
	m_Program->SetUniform3f("Ka", materialInfo.Ka);
	m_Program->SetUniform3f("Kd", materialInfo.Kd);
	m_Program->SetUniform3f("Ks", materialInfo.Ks);


	int width, height, bpp;
	unsigned char* data = stbi_load("res/textures/tex1.png", &width, &height, &bpp, 4);

	// Copy file to OpenGL
	unsigned int texId;

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height); // allocate memory
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (data)
	{
		stbi_image_free(data);
	}

	// Set the Tex1 sampler uniform to texture unit 0
	int loc = glGetUniformLocation(((Bindable*)m_Program)->GetId(), "Texture");
	if (loc >= 0)
	{
		glUniform1i(loc, 0); // GL_TEXTURE0
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);

	// create two smaplers
	unsigned int samplers[2];
	glGenSamplers(2, samplers);

	linearSampler = samplers[0];
	nearestSampler = samplers[1];

	// set up linearSampler for linear interpolation
	glSamplerParameteri(linearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(linearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// set up nearestSampler for nearest-neighbor sampling
	glSamplerParameteri(nearestSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(nearestSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	m_Program->Unbind();
	m_VBA->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void core::Example_4_8_Window::Loop(float dt)
{
	time += dt;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_Program->Bind();
	m_VBA->Bind();

	float radius = 2.5f;

	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), radius * glm::cos(time), radius * glm::cos(time));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniform3f("u_CameraPos", cameraPos);


	// bind nearestSampler to texture
	glBindSampler(0, nearestSampler);
	m_Program->SetUniformMat4f("u_Model", glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f))));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCount * 3));

	// bind linearSampler to texture
	glBindSampler(0, linearSampler);
	m_Program->SetUniformMat4f("u_Model", glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f))));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCount * 3));

	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();
}

void core::Example_4_8_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_4_8_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_4_8_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_4_8_Window::OnScrollAction(double xoffset, double yoffset)
{
}
