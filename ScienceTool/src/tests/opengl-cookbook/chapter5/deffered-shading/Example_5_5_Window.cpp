#include "Example_5_5_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

#include "stb_image/stb_image.h"

core::Example_5_5_Window::Example_5_5_Window(const int w, const int h, const char* l) : Window(w, h, l),
m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0)))
{
	std::cout << "Example_5_5_Window Constructor " << std::endl;
	this->texData = new float[w * h * 4];
}

core::Example_5_5_Window::~Example_5_5_Window()
{
	delete m_Program_Obj;
	delete m_VBA_Obj;
	delete m_VBO_Obj;
	delete m_VBL_Obj;

	delete m_VBA_Plane;
	delete m_VBO_Plane;
	delete m_VBL_Plane;

	delete m_Grid;

	delete[] texData;
}

void core::Example_5_5_Window::Init()
{
	std::cout << "Example_5_5_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	// object parsing and binding

	{
		m_Program_Obj = new Program("res/shaders/opengl-cookbook/chapter5/ex_5/DefferedShading.shader");

		parser::Vertices* vertsObj = parser::obj::ObjParser::Parse_P_N_T("res/obj/torus_uv.objj");
		this->trianglesCountObj = vertsObj->GetTrianglesCount();

		m_VBA_Obj = new VertexArray();
		m_VBO_Obj = new VertexBuffer(vertsObj->GetData(), vertsObj->GetSize());
		m_VBL_Obj = new VertexBufferLayout();

		delete vertsObj;

		m_VBL_Obj->Push<float>(3);
		m_VBL_Obj->Push<float>(3);
		m_VBL_Obj->Push<float>(2);

		m_VBA_Obj->AddBuffer(*m_VBO_Obj, *m_VBL_Obj);

		m_Program_Obj->Bind();

		m_Program_Obj->SetUniformMat4f("u_Projection", this->m_ProjMat);
		m_Program_Obj->SetUniformMat4f("u_Model", this->m_ModelMat);

		this->_pass1_SubroutineIndex = glGetSubroutineIndex(
			((opengl::Bindable*) m_Program_Obj)->GetId(),
			GL_FRAGMENT_SHADER,
			"pass1"
		);

		this->_pass2_SubroutineIndex = glGetSubroutineIndex(
			((opengl::Bindable*) m_Program_Obj)->GetId(),
			GL_FRAGMENT_SHADER,
			"pass2"
		);

		struct MaterialInfo {
			glm::vec3 Ka;		 // ambient reflectivity
			glm::vec3 Kd;		 // diffuse reflectivity
			glm::vec3 Ks;		 // specular reflectivity
			float Shininess; // specular shininess factor
		};

		MaterialInfo materialInfo;
		materialInfo.Kd = glm::vec3(1.0f, 1.0f, 0.0f);

		float radius = 3.0f;

		// SHADING MODEL
		m_Program_Obj->SetUniform3f("Light.Position", glm::vec3(-radius, 0.0f, 0.0f));
		m_Program_Obj->SetUniform3f("Light.Intensity", glm::vec3(0.677f, 0.66f, 0.45f));

		m_Program_Obj->SetUniform3f("Material.Kd", glm::vec3(1.0f, 0.0, 0.0f));
		
		
	}


	// plane parsing and binding
	{
		parser::Vertices* vertsPlane = parser::obj::ObjParser::Parse_P_N_T("res/obj/plane3_uv.objj");
		this->trianglesCountPlane = vertsPlane->GetTrianglesCount();

		m_VBA_Plane = new VertexArray();
		m_VBO_Plane = new VertexBuffer(vertsPlane->GetData(), vertsPlane->GetSize());
		m_VBL_Plane = new VertexBufferLayout();

		delete vertsPlane;

		m_VBL_Plane->Push<float>(3);
		m_VBL_Plane->Push<float>(3);
		m_VBL_Plane->Push<float>(2);

		m_VBA_Plane->AddBuffer(*m_VBO_Plane, *m_VBL_Plane);
	}


	// Create and bind the FBO
	glGenFramebuffers(1, &this->deferredFBOId);
	glBindFramebuffer(GL_FRAMEBUFFER, this->deferredFBOId);

	// The depth buffer
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width, this->height);



	// The position, normal and color buffers
	CreateGeometryBufTex(GL_TEXTURE0, GL_RGB32F, posTex);  // Position
	CreateGeometryBufTex(GL_TEXTURE1, GL_RGB32F, normTex); // Normal
	CreateGeometryBufTex(GL_TEXTURE2, GL_RGB8, colorTex);  // Color

	// Attach the images to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorTex, 0);

	unsigned int drawBuffers[] = {GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(4, drawBuffers);

	unsigned int result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "High Resolution FBO is complete" << std::endl;
	}
	else
	{
		std::cout << "High Resolution FBO error: " << result << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_Program_Obj->SetUniform1i("PositionTex", 0);
	m_Program_Obj->SetUniform1i("NormalTex", 1);
	m_Program_Obj->SetUniform1i("ColorTex", 2);

	m_Program_Obj->Unbind();
	m_VBA_Obj->Unbind();
	m_VBA_Plane->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void core::Example_5_5_Window::CreateGeometryBufTex(unsigned int texUnit, unsigned int format, unsigned int& texId)
{
	glActiveTexture(texUnit);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexStorage2D(GL_TEXTURE_2D, 1, format, this->width, this->height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void core::Example_5_5_Window::Loop(float dt)
{

	time += dt;

	//time = 0;

	float radius = 3.5f;

	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), radius * glm::cos(time), radius * glm::cos(time));
	glm::mat4 viewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program_Obj->Bind();
	m_VBA_Obj->Bind();

	m_Program_Obj->SetUniformMat4f("u_Projection", this->m_ProjMat);
	m_Program_Obj->SetUniformMat4f("u_View", viewMat);
	m_Program_Obj->SetUniformMat4f("u_Model", this->m_ModelMat);


	/* PASS 1 */
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass1_SubroutineIndex);

	glBindFramebuffer(GL_FRAMEBUFFER, this->deferredFBOId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountObj * 4));


	/*glActiveTexture(GL_TEXTURE0);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData);

	double sum = 0.0f;
	int size = this->width * this->height * 3;

	for (int i = 0; i < size; i++)
	{
		sum += texData[i];

	//	std::cout << texData[i * 3 + 0] << ", " << texData[i * 3 + 1] << ", " << texData[i * 3 + 2] << ", " << texData[i * 3 + 3] << std::endl;
	}
	

	std::cout << "sum = " << sum << std::endl;
	*/


	
	// For all next passes - draw plane with identity matrices

	m_Program_Obj->Bind();
	m_VBA_Plane->Bind();

	m_Program_Obj->SetUniformMat4f("u_Projection", glm::mat4(1.0f));
	m_Program_Obj->SetUniformMat4f("u_View", glm::mat4(1.0f));
	m_Program_Obj->SetUniformMat4f("u_Model", glm::mat4(1.0f));


	/* PASS 2 */

	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass2_SubroutineIndex);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountPlane * 3));


}

void core::Example_5_5_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_5_5_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_5_5_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_5_5_Window::OnScrollAction(double xoffset, double yoffset)
{
}
