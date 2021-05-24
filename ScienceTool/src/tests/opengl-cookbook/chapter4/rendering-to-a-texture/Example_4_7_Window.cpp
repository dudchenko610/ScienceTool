#include "Example_4_7_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

#include "stb_image/stb_image.h"

core::Example_4_7_Window::Example_4_7_Window(const int w, const int h, const char* l) : Window(w, h, l),

	//	m_ProjMat(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -0.1f, 10.0f)),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ViewMat(glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0))),

	trianglesCount(0)

{
	std::cout << "Example_4_7_Window Constructor " << std::endl;

}

core::Example_4_7_Window::~Example_4_7_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;

	delete m_Grid;
}

void core::Example_4_7_Window::Init()
{
	std::cout << "Example_4_7_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	parser::Vertices* verts = parser::obj::ObjParser::Parse_P_N_T("res/obj/cube_uv.objj");
	this->trianglesCount = verts->GetTrianglesCount();

	m_Program = new Program("res/shaders/opengl-cookbook/chapter4/ex_7/FBO_Example.shader");
	m_VBA = new VertexArray();
	m_VBO = new VertexBuffer(verts->GetData(), verts->GetSize());
	m_VBL = new VertexBufferLayout();

	delete verts;

	m_VBL->Push<float>(3);
	m_VBL->Push<float>(3);
	m_VBL->Push<float>(2);

	m_VBA->AddBuffer(*m_VBO, *m_VBL);


	m_Program->Bind();
//	m_Program->SetUniformMat4f("u_Projection", m_ProjMat);
//	m_Program->SetUniformMat4f("u_View", m_ViewMat);
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

	m_Program->SetUniform3f("Light.position", glm::vec3(-radius, 0.0f, 0.0f));
	m_Program->SetUniform3f("Light.intensity", glm::vec3(1.0f, 0.4f, 0.4f));


	MaterialInfo materialInfo;
	materialInfo.Ka = glm::vec3(0.8f, 0.8f, 0.8f);
	materialInfo.Kd = glm::vec3(0.3f, 0.4f, 0.4f);
	materialInfo.Ks = glm::vec3(0.5f, 0.5f, 0.5f);
	materialInfo.Shininess = 8;

	m_Program->SetUniform1f("Shininess", materialInfo.Shininess);
	m_Program->SetUniform3f("Ka", materialInfo.Ka);
	m_Program->SetUniform3f("Kd", materialInfo.Kd);
	m_Program->SetUniform3f("Ks", materialInfo.Ks);

	/*** 1_ST STEP ***/

	// Generate and bind the framebuffer
	glGenFramebuffers(1, &fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

	// Create the texture object (someone to that we will draw)
	unsigned int renderTex;

	glActiveTexture(GL_TEXTURE0); // Use texture unit 0
	glGenTextures(1, &renderTex);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512); // Allocate memory
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0); // 0 - mipmap level
	

	// Create the depth buffer
	unsigned int depthBuf;
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512); // most likely it is memory allocation

	// Bind the depth buffer to the FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf); // to currently bound

	// Set the target for the fragment shader outputs
	unsigned int drawBufs[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBufs);

	// The shader's output variables are assigned to the attachments of the FBO using
	// glDrawBuffers. The second argument to glDrawBuffers is an array indicating the FBO
	// buffers to be associated with the output variables.

	// Unbind the framebuffer, and revert to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is complete" << std::endl;
	}
	else {
		std::cout << "Framebuffer error: " << result << std::endl;
	}

	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	

	/*** 2_ND STEP ***/

	// One pixel white texture
	unsigned int whiteTexHandle;
	unsigned char whiteTex[] = { 255, 255, 255, 255 };
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &whiteTexHandle);
	glBindTexture(GL_TEXTURE_2D, whiteTexHandle);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1); // memory allocation
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTex); // copy data

	m_Program->Unbind();
	m_VBA->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void core::Example_4_7_Window::Loop(float dt)
{
	time += dt;

	m_Program->Bind();
	m_VBA->Bind();


	

	/* 1. DRAW TO FRAMsEBUFFER */

	// Bind to texture's FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears currently bound FBO
	glViewport(0, 0, 512, 512); // Viewport for the texture

	// Use the "white" texture here
	m_Program->SetUniform1i("Texture", 1);
	m_Program->SetUniform3f("Light.intensity", glm::vec3(0.1f, 0.1f, 1.0f));

	float radius = 1.5f;

	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), radius * glm::cos(time), radius * glm::cos(time));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniform3f("u_CameraPos", cameraPos);

	// Setup the projection matrix and view matrix
	// for the scene to be rendered to the texture here.
	// (Don't forget to match aspect ratio of the viewport.)
	m_Program->SetUniformMat4f("u_Projection", glm::mat4(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f)) );


	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCount * 3));

	/* 1. DRAW TO FRAMEBUFFER */

	glFlush();

	/* 2. DRAW TO SCREEN */

	// Unbind texture's FBO (back to default FB)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, this->width, this->height); // Viewport for main window

	// Use the texture that is associated with the FBO
	m_Program->SetUniform1i("Texture", 0); // we use texture, that was filled before
	m_Program->SetUniformMat4f("u_Projection", glm::mat4(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)));
	m_Program->SetUniform3f("Light.intensity", glm::vec3(0.5f, 0.5f, 1.0f));


	radius = 2.5f;

	cameraPos = glm::vec3(radius * glm::sin(time), radius * glm::cos(time), radius * glm::cos(time));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniform3f("u_CameraPos", cameraPos);


	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCount * 3));

	/* 2. DRAW TO SCREEN */

	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();
}

void core::Example_4_7_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_4_7_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_4_7_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_4_7_Window::OnScrollAction(double xoffset, double yoffset)
{
}
