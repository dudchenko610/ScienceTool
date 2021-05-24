#include "Example_5_4_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

#include "stb_image/stb_image.h"


float core::Example_5_4_Window::gauss(float x, float sigmaSquared)
{
	return (1.0f / glm::sqrt(2 * glm::pi<float>() * sigmaSquared)) * glm::pow(glm::e<float>(), (-x * x / (2 * sigmaSquared)));
}

core::Example_5_4_Window::Example_5_4_Window(const int w, const int h, const char* l) : Window(w, h, l),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0)))
{
	std::cout << "Example_5_4_Window Constructor " << std::endl;

}

core::Example_5_4_Window::~Example_5_4_Window()
{
	delete m_Program_Obj;
	delete m_VBA_Obj;
	delete m_VBO_Obj;
	delete m_VBL_Obj;

	delete m_VBA_Plane;
	delete m_VBO_Plane;
	delete m_VBL_Plane;

	delete m_Grid;
}

void core::Example_5_4_Window::Init()
{
	std::cout << "Example_5_4_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	// object parsing and binding

	{
		m_Program_Obj = new Program("res/shaders/opengl-cookbook/chapter5/ex_4/BloomEffect.shader");

		parser::Vertices* vertsObj = parser::obj::ObjParser::Parse_P_N_T("res/obj/sphere_uv.objj");
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

		this->_pass3_SubroutineIndex = glGetSubroutineIndex(
			((opengl::Bindable*) m_Program_Obj)->GetId(),
			GL_FRAGMENT_SHADER,
			"pass3"
		);

		this->_pass4_SubroutineIndex = glGetSubroutineIndex(
			((opengl::Bindable*) m_Program_Obj)->GetId(),
			GL_FRAGMENT_SHADER,
			"pass4"
		);

		this->_pass5_SubroutineIndex = glGetSubroutineIndex(
			((opengl::Bindable*) m_Program_Obj)->GetId(),
			GL_FRAGMENT_SHADER,
			"pass5"
		);

		struct MaterialInfo {
			glm::vec3 Ka;		 // ambient reflectivity
			glm::vec3 Kd;		 // diffuse reflectivity
			glm::vec3 Ks;		 // specular reflectivity
			float Shininess; // specular shininess factor
		};

		MaterialInfo materialInfo;
		materialInfo.Ka = glm::vec3(0.3f, 0.3f, 0.3f);
		materialInfo.Kd = glm::vec3(0.7f, 0.7f, 0.7f);
		materialInfo.Ks = glm::vec3(0.6f, 0.5f, 0.6f);
		materialInfo.Shininess = 6;

		float radius = 3.0f;

		// SHADING MODEL
		m_Program_Obj->SetUniform3f("Light.Position", glm::vec3(-radius, 0.0f, 0.0f));
		m_Program_Obj->SetUniform3f("Light.Intensity", glm::vec3(0.677f, 0.66f, 0.45f));

		m_Program_Obj->SetUniform1f("Material.Shininess", materialInfo.Shininess);
		m_Program_Obj->SetUniform3f("Material.Ka", materialInfo.Ka);
		m_Program_Obj->SetUniform3f("Material.Kd", materialInfo.Kd);
		m_Program_Obj->SetUniform3f("Material.Ks", materialInfo.Ks);

		// EDGE DETECTING
		m_Program_Obj->SetUniform1f("LumThresh", 0.60f);

		// GAUSSIAN BLUR FILTER
		float weights[5], sum, sigma2 = 12.0f;
		weights[0] = gauss(0, sigma2); // The 1-D Gaussian function
		sum = weights[0];

		for (int i = 1; i < 5; i++)
		{
			weights[i] = gauss(i, sigma2);
			sum += 2 * weights[i];
		}

		// Normalize the weights and set the uniform
		for (int i = 0; i < 5; i++)
		{
			weights[i] = weights[i] / sum;
		}

		m_Program_Obj->SetUniform1f("Weight[0]", weights[0]);
		m_Program_Obj->SetUniform1f("Weight[1]", weights[1]);
		m_Program_Obj->SetUniform1f("Weight[2]", weights[2]);
		m_Program_Obj->SetUniform1f("Weight[3]", weights[3]);
		m_Program_Obj->SetUniform1f("Weight[4]", weights[4]);

		// HDR
		float logAve = 0.5f; 
		m_Program_Obj->SetUniform1f("AveLum", logAve); // should be computed in Loop method summing all pixels of texture

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
	this->InitHighlightFBO();
	this->InitHighlightTextures();

	// Unbind the framebuffer, and revert to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	m_Program_Obj->Unbind();
	m_VBA_Obj->Unbind();
	m_VBA_Plane->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

}

void core::Example_5_4_Window::InitHighResolutionFBO()
{
	// Framebuffer that has the same dimensions as the main window
	// Gen FBO
	glGenFramebuffers(1, &this->highResolutionFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, this->highResolutionFboId);

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
	glGenTextures(1, &highResTexId);
	glBindTexture(GL_TEXTURE_2D, highResTexId);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, this->width, this->height); // Allocate memory
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind the texture to the currently bound FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, highResTexId, 0); // 0 - mipmap level

	unsigned int result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "High Resolution FBO is complete" << std::endl;
	}
	else
	{
		std::cout << "High Resolution FBO error: " << result << std::endl;
	}

}

void core::Example_5_4_Window::InitHighlightFBO()
{
	// Gen FBO
	glGenFramebuffers(1, &this->highlightFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, this->highlightFboId);

	// Set the target for the fragment shader outputs
	unsigned int drawBufs[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBufs);

}

void core::Example_5_4_Window::InitHighlightTextures()
{
	// Create the texture object (someone we will draw to)
	glActiveTexture(GL_TEXTURE1); // Use texture unit 1
	glGenTextures(1, &this->highlightTex1Id);
	glBindTexture(GL_TEXTURE_2D, highlightTex1Id);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, this->width , this->height); // Allocate memory
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	// Create the texture object (someone we will draw to)
	glActiveTexture(GL_TEXTURE2); // Use texture unit 2
	glGenTextures(1, &this->highlightTex2Id);
	glBindTexture(GL_TEXTURE_2D, highlightTex2Id);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, this->width, this->height); // Allocate memory
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void core::Example_5_4_Window::Loop(float dt)
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

	/* PASS 1 render the scene to the framebuffer with a high-res backing texture */
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass1_SubroutineIndex);

	glBindFramebuffer(GL_FRAMEBUFFER, this->highResolutionFboId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountObj * 3));

	/*float* texData = new float[width * height * 3];

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->highResTexId);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData);

	double sum = 0.0f;
	int size = this->width * this->height;

	for (int i = 0; i < size; i++)
	{
		sum += texData[i];

		
	}

	delete texData;*/


	m_Grid->BindAndDraw(this->m_ProjMat, viewMat);
	m_Grid->Unbind();


	// For all next passes - draw plane with identity matrices

	m_Program_Obj->Bind();
	m_VBA_Plane->Bind();

	m_Program_Obj->SetUniformMat4f("u_Projection", glm::mat4(1.0f));
	m_Program_Obj->SetUniformMat4f("u_View", glm::mat4(1.0f));
	m_Program_Obj->SetUniformMat4f("u_Model", glm::mat4(1.0f));



	/* PASS 2  switch to a framebuffer containing a high-res texture that is smaller than the size of the full render  */
	//         Draw a full screen quad to initiate the fragement shader for each pixel, and in the fragment shader sample 
	//         from the high - res texture, and write only	those values that are larger than LumThresh.Otherwise, color the pixel black.
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass2_SubroutineIndex);

	glBindFramebuffer(GL_FRAMEBUFFER, this->highlightFboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->highlightTex1Id, 0); // Bind the highlight texture 1 to the currently bound FBO
	glClear(GL_COLOR_BUFFER_BIT);

	// Set pass high-resolution texture as an input and draw to highlight texture
	m_Program_Obj->SetUniform1i("InputTexture", 0);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountPlane * 3));



	/* PASS 3  Gaussian blur filter (Step 1) */
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass3_SubroutineIndex);

	glBindFramebuffer(GL_FRAMEBUFFER, this->highlightFboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->highlightTex2Id, 0); // Bind the highlight texture 2 to the currently bound FBO
	glClear(GL_COLOR_BUFFER_BIT);

	// Set preceding output texture an an input
	m_Program_Obj->SetUniform1i("InputTexture", 1);
	
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountPlane * 3));



	/* PASS 4  Gaussian blur filter (Step 2) */
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass4_SubroutineIndex);
	// Set preceding output texture an an input
	m_Program_Obj->SetUniform1i("InputTexture", 2);

	glBindFramebuffer(GL_FRAMEBUFFER, this->highlightFboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->highlightTex1Id, 0); // Bind the highlight texture 1to the currently bound FBO
	glClear(GL_COLOR_BUFFER_BIT);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountPlane * 3));


	/* PASS 5 HDR and combine colors */
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass5_SubroutineIndex);

	// Revert to default FBO (screen)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// apply linear filtering
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->highlightTex1Id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_Program_Obj->SetUniform1i("InputTexture", 0);           // high-resolution texture 
	m_Program_Obj->SetUniform1i("HighlightResultTexture", 1); // output of blur stage

	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountPlane * 3));
	
	
	// apply nearest filtering back
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->highlightTex1Id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);



}

void core::Example_5_4_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_5_4_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_5_4_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_5_4_Window::OnScrollAction(double xoffset, double yoffset)
{
}
