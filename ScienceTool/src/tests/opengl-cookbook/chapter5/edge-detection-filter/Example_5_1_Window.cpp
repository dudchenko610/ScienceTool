#include "Example_5_1_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

#include "stb_image/stb_image.h"

core::Example_5_1_Window::Example_5_1_Window(const int w, const int h, const char* l) : Window(w, h, l),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0)))
{
	std::cout << "Example_5_1_Window Constructor " << std::endl;
}

core::Example_5_1_Window::~Example_5_1_Window()
{
	delete m_Program_Obj;
	delete m_VBA_Obj;
	delete m_VBO_Obj;
	delete m_VBL_Obj;

	delete m_Program_Plane;
	delete m_VBA_Plane;
	delete m_VBO_Plane;
	delete m_VBL_Plane;

	delete m_Grid;
}

void core::Example_5_1_Window::Init()
{
	std::cout << "Example_5_1_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	
	// object parsing and binding

	{
		m_Program_Obj = new Program("res/shaders/opengl-cookbook/chapter5/ex_1/EdgeDetectionFilter.shader");

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


		struct MaterialInfo {
			glm::vec3 Ka;		 // ambient reflectivity
			glm::vec3 Kd;		 // diffuse reflectivity
			glm::vec3 Ks;		 // specular reflectivity
			float Shininess; // specular shininess factor
		};

		MaterialInfo materialInfo;
		materialInfo.Ka = glm::vec3(0.8f, 0.8f, 0.8f);
		materialInfo.Kd = glm::vec3(0.3f, 0.4f, 0.4f);
		materialInfo.Ks = glm::vec3(0.5f, 0.5f, 0.5f);
		materialInfo.Shininess = 8;

		float radius = 3.0f;

		m_Program_Obj->SetUniform3f("Light.Position", glm::vec3(-radius, 0.0f, 0.0f));
		m_Program_Obj->SetUniform3f("Light.Intensity", glm::vec3(1.0f, 0.4f, 0.4f));

		m_Program_Obj->SetUniform1f("Material.Shininess", materialInfo.Shininess);
		m_Program_Obj->SetUniform3f("Material.Ka", materialInfo.Ka);
		m_Program_Obj->SetUniform3f("Material.Kd", materialInfo.Kd);
		m_Program_Obj->SetUniform3f("Material.Ks", materialInfo.Ks);

		/*
		m_Program_Obj->SetUniform1i("Width", this->width);
		m_Program_Obj->SetUniform1i("Height", this->height);
		*/

		m_Program_Obj->SetUniform1f("EdgeThreshold", 0.01f);

		this->pass1SubroutineIndex = glGetSubroutineIndex(
			((opengl::Bindable*) m_Program_Obj)->GetId(),
			GL_FRAGMENT_SHADER,
			"pass1"
		);

		this->pass2SubroutineIndex = glGetSubroutineIndex(
			((opengl::Bindable*) m_Program_Obj)->GetId(),
			GL_FRAGMENT_SHADER,
			"pass2"
		);


	}


	// plane parsing and binding

	{
		m_Program_Plane = new Program("res/shaders/opengl-cookbook/chapter5/ex_1/RenderPlane.shader");

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

		m_Program_Plane->Bind();

		m_Program_Plane->SetUniformMat4f("u_Projection", this->m_ProjMat);
		m_Program_Plane->SetUniformMat4f("u_Model", this->m_ModelMat);

		

	}


	// Framebuffer that has the same dimensions as the main window
	glGenFramebuffers(1, &fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

	// Create the texture object (someone to that we will draw)
	unsigned int renderTex;

	glActiveTexture(GL_TEXTURE0); // Use texture unit 0
	glGenTextures(1, &renderTex);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, this->width, this->height); // Allocate memory
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0); // 0 - mipmap level

	// Create the depth buffer
	unsigned int depthBuf;
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width, this->height); // most likely it is memory allocation

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

	unsigned int result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) 
	{
		std::cout << "Framebuffer is complete" << std::endl;
	}
	else 
	{
		std::cout << "Framebuffer error: " << result << std::endl;
	}

	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_Program_Plane->Unbind();
	m_Program_Obj->Unbind();
	m_VBA_Obj->Unbind();
	m_VBA_Plane->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

}

void core::Example_5_1_Window::Loop(float dt)
{
	time += dt;

	float radius = 3.5f;

	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), radius * glm::cos(time), radius * glm::cos(time));
	glm::mat4 viewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program_Obj->Bind();
	m_VBA_Obj->Bind();

	// PASS 1

	/* 1. Select FBO and clear the color/depth buffers */
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* 2. Select pass1 subroutine function */
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->pass1SubroutineIndex);

	/* 3. Set up matrices and draw the scene */
	m_Program_Obj->SetUniformMat4f("u_View", viewMat);
	m_Program_Obj->SetUniform3f("u_CameraPos", cameraPos);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountObj * 3));

	m_Grid->BindAndDraw(this->m_ProjMat, viewMat);
	m_Grid->Unbind();

	// PASS 2

	/* 1. Deselect the FBO (revert to default) and clear color/ depth buffers */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* 2. Select pass2 subroutine function */
	m_Program_Obj->Bind();
	m_VBA_Plane->Bind();

	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->pass2SubroutineIndex);

	/* 3. Draw filled texture */

	m_Program_Obj->SetUniform3f("u_CameraPos", glm::vec3(0.0f, 0.0f, 1.5f));
	m_Program_Obj->SetUniformMat4f("u_View", glm::lookAt(glm::vec3(0.0f, 0.0f, 1.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountPlane * 3));


}

void core::Example_5_1_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_5_1_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_5_1_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_5_1_Window::OnScrollAction(double xoffset, double yoffset)
{
}
