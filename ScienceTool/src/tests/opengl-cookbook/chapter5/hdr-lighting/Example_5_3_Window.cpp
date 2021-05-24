#include "Example_5_3_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

#include "stb_image/stb_image.h"


core::Example_5_3_Window::Example_5_3_Window(const int w, const int h, const char* l) : Window(w, h, l),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0)))
{
	std::cout << "Example_5_3_Window Constructor " << std::endl;

	this->texData = new float[ w * h * 3 ];
}

core::Example_5_3_Window::~Example_5_3_Window()
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

void core::Example_5_3_Window::Init()
{
	std::cout << "Example_5_3_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	// object parsing and binding

	{
		m_Program_Obj = new Program("res/shaders/opengl-cookbook/chapter5/ex_3/HDR_Lighting.shader");

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
		materialInfo.Ka = glm::vec3(0.3f, 0.3f, 0.3f);
		materialInfo.Kd = glm::vec3(0.7f, 0.7f, 0.7f);
		materialInfo.Ks = glm::vec3(1.5f, 1.9f, 1.2f);
		materialInfo.Shininess = 8;

		float radius = 3.0f;

		m_Program_Obj->SetUniform3f("Light.Position", glm::vec3(-radius, 0.0f, 0.0f));
		m_Program_Obj->SetUniform3f("Light.Intensity", glm::vec3(1.5f, 1.2f, 1.0f));

		m_Program_Obj->SetUniform1f("Material.Shininess", materialInfo.Shininess);
		m_Program_Obj->SetUniform3f("Material.Ka", materialInfo.Ka);
		m_Program_Obj->SetUniform3f("Material.Kd", materialInfo.Kd);
		m_Program_Obj->SetUniform3f("Material.Ks", materialInfo.Ks);

		/*
		m_Program_Obj->SetUniform1i("Width", this->width);
		m_Program_Obj->SetUniform1i("Height", this->height);
		*/

		

		m_Program_Obj->SetUniform1f("Weight[0]", 0);

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

	this->InitHighResolutionFBO();

	// Unbind the framebuffer, and revert to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	m_Program_Obj->Unbind();
	m_VBA_Obj->Unbind();
	m_VBA_Plane->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

}

void core::Example_5_3_Window::InitHighResolutionFBO()
{
	// Framebuffer that has the same dimensions as the main window
	// Gen FBO
	glGenFramebuffers(1, &this->fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, this->fboHandle);

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


	// Create the texture object (someone we will draw to)
	glActiveTexture(GL_TEXTURE0); // Use texture unit 0
	glGenTextures(1, &hdrTextureId);
	glBindTexture(GL_TEXTURE_2D, hdrTextureId);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, this->width, this->height); // Allocate memory
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTextureId, 0); // 0 - mipmap level

	unsigned int result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer 1 is complete" << std::endl;
	}
	else
	{
		std::cout << "Framebuffer 1 error: " << result << std::endl;
	}

}


void core::Example_5_3_Window::Loop(float dt)
{
	time += dt;

	float radius = 3.5f;

	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), radius * glm::cos(time), radius * glm::cos(time));
	glm::mat4 viewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program_Obj->Bind();
	m_VBA_Obj->Bind();

	m_Program_Obj->SetUniformMat4f("u_Projection", this->m_ProjMat);
	m_Program_Obj->SetUniformMat4f("u_View", viewMat);
	m_Program_Obj->SetUniformMat4f("u_Model", this->m_ModelMat);

	m_Program_Obj->SetUniform3f("u_CameraPos", cameraPos);

	// PASS 1 render the scene to high resolution texture

	glBindFramebuffer(GL_FRAMEBUFFER, this->fboHandle);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass1_SubroutineIndex);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountObj * 3));


	m_Grid->BindAndDraw(this->m_ProjMat, viewMat);
	m_Grid->Unbind();
	
	// PASS 2 Compute the log average luminance of the pixels in the texture

	
	m_Program_Obj->Bind();
	m_VBA_Plane->Bind();
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass2_SubroutineIndex);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, hdrTextureId);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData);

//	float sum = 0.0f;
//	int size = this->width * this->height;

	/*for (int i = 0; i < size; i ++)
	{
		float lum = glm::dot(glm::vec3(texData[i * 3 + 0], texData[i * 3 + 1], texData[i * 3 + 2]), glm::vec3(0.2126f, 0.7152f, 0.0722f));
		sum += glm::log(lum + 0.00001f);
	}*/

	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->hdrTextureId);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData);

	double sum = 0.0f;
	int size = this->width * this->height;

	for (int i = 0; i < size; i++)
	{
		sum += texData[i];

		std::cout << texData[i * 3 + 0] << ", " << texData[i * 3 + 1] << ", " << texData[i * 3 + 2] << std::endl;
	}*/


	float logAve = 0.9f;// glm::exp(sum / size);
	m_Program_Obj->SetUniform1f("AveLum", logAve);

	m_Program_Obj->SetUniformMat4f("u_Projection", glm::mat4(1.0f));
	m_Program_Obj->SetUniformMat4f("u_View", glm::mat4(1.0f));
	m_Program_Obj->SetUniformMat4f("u_Model", glm::mat4(1.0f));
	m_Program_Obj->SetUniform1i("HdrTex", 0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountPlane * 3));
	

}

void core::Example_5_3_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_5_3_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_5_3_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_5_3_Window::OnScrollAction(double xoffset, double yoffset)
{
}
