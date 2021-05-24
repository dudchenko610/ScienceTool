#include "Example_7_1_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"


core::Example_7_1_Window::Example_7_1_Window(const int w, const int h) : Window(w, h, "Example_7_1_Window"),

	//	m_ProjMat(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -0.1f, 10.0f)),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	shadowMapWidth(256),
	shadowMapHeight(256),

	lightInfo(
		{
			glm::vec3(0.0f, 6.0f, 0.0f), // light position
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
	std::cout << "Example_7_1_Window Constructor " << std::endl;

}

core::Example_7_1_Window::~Example_7_1_Window()
{
	delete m_Program;

	delete m_VBA_Torus;
	delete m_VBO_Torus;
	delete m_VBL_Torus;

	delete m_VBA_Plane;
	delete m_VBO_Plane;
	delete m_VBL_Plane;

	delete m_Grid;
}

void core::Example_7_1_Window::Init()
{
	std::cout << "Example_7_1_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	//m_Program = new Program("res/shaders/opengl-cookbook/chapter2/ex_2/Flat_Shading.shader");
	m_Program = new Program("res/shaders/opengl-cookbook/chapter7/ex_1", "ShadowMaps_VS.glsl", "ShadowMaps_FS.glsl");
	
	{
		parser::Vertices* verts_Torus = parser::obj::ObjParser::Parse("res/obj/torus_knot.objj");
		this->trianglesCountTorus = verts_Torus->GetTrianglesCount();

		m_VBA_Torus = new VertexArray();
		m_VBO_Torus = new VertexBuffer(verts_Torus->GetData(), verts_Torus->GetSize());
		m_VBL_Torus = new VertexBufferLayout();

		delete verts_Torus;

		m_VBL_Torus->Push<float>(3);
		m_VBL_Torus->Push<float>(3);

		m_VBA_Torus->AddBuffer(*m_VBO_Torus, *m_VBL_Torus);

	}

	{
		parser::Vertices* verts_Plane = parser::obj::ObjParser::Parse("res/obj/plane.objj");
		this->trianglesCountPlane = verts_Plane->GetTrianglesCount();

		m_VBA_Plane = new VertexArray();
		m_VBO_Plane = new VertexBuffer(verts_Plane->GetData(), verts_Plane->GetSize());
		m_VBL_Plane = new VertexBufferLayout();


		delete verts_Plane;

		m_VBL_Plane->Push<float>(3);
		m_VBL_Plane->Push<float>(3);
			  
		m_VBA_Plane->AddBuffer(*m_VBO_Plane, *m_VBL_Plane);
	}
	

	m_Program->Bind();
	
	m_Program->SetUniform3f("Light.Position", this->lightInfo.position);
	m_Program->SetUniform3f("Light.La", this->lightInfo.La);
	m_Program->SetUniform3f("Light.Ld", this->lightInfo.Ld);
	m_Program->SetUniform3f("Light.Ls", this->lightInfo.Ls);

	m_Program->SetUniform1f("Material.Shininess", this->materialInfo.Shininess);
	m_Program->SetUniform3f("Material.Ka", this->materialInfo.Ka);
	m_Program->SetUniform3f("Material.Kd", this->materialInfo.Kd);
	m_Program->SetUniform3f("Material.Ks", this->materialInfo.Ks);



	this->pass1SubroutineIndex = glGetSubroutineIndex(
		((opengl::Bindable*) m_Program)->GetId(),
		GL_FRAGMENT_SHADER,
		"recordDepth"
	);

	this->pass2SubroutineIndex = glGetSubroutineIndex(
		((opengl::Bindable*) m_Program)->GetId(),
		GL_FRAGMENT_SHADER,
		"shadeWithShadow"
	);

	glm::mat4 shadowBias = glm::mat4(
		glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
	);

	this->m_ProjMat_Light = glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	this->m_ViewMat_Light = glm::lookAt(this->lightInfo.position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	this->m_BPV_Light = shadowBias * this->m_ProjMat_Light * this->m_ViewMat_Light;

	this->SetupFBOWithDepthBufferOnly();

	m_Program->SetUniform1i("ShadowMap", 0);

	m_Program->Unbind();
	m_VBA_Torus->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


}

void core::Example_7_1_Window::SetupFBOWithDepthBufferOnly()
{
	float border[] = { 1.0f, 0.0f, 0.0f, 0.0f };

	// The shadowmap texture
	glGenTextures(1, &this->depthTexId);
	glBindTexture(GL_TEXTURE_2D, this->depthTexId);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, this->shadowMapWidth, this->shadowMapHeight);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

	// THE MOST IMPORTANT PART
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	// Assign the shadow map to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->depthTexId);

	// Create and set up the FBO
	glGenFramebuffers(1, &this->shadowFBOId);
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowFBOId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowFBOId, 0);

	unsigned int drawBuffers[] = { GL_NONE };
	glDrawBuffers(1, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer is complete.\n");
	}
	else {
		printf("Framebuffer is not complete.\n");
	}

	// Revert to the default framebuffer for now
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



}


void core::Example_7_1_Window::Loop(float dt)
{
	time += dt;

	m_Program->Bind();

	/* PASS 1 */

		// 1
	m_Program->SetUniformMat4f("u_Projection", this->m_ProjMat_Light);
	m_Program->SetUniformMat4f("u_View", this->m_ViewMat_Light);

		// 2
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowFBOId);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// 3
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, this->shadowMapWidth, this->shadowMapHeight);

		// 4
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->pass1SubroutineIndex);

		// 5
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.5f, 10.0f);

		// 6
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::rotate(model, time, glm::vec3(0.0f, 0.0f, 1.0f));
		
		m_VBA_Torus->Bind();
		m_Program->SetUniformMat4f("u_Model", model);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountTorus * 3));
	}

	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		m_VBA_Plane->Bind();
		m_Program->SetUniformMat4f("u_Model", model);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountPlane * 3));
	}

	glCullFace(GL_BACK);

	/* PASS 2 */

		// 1
	float radius = 3.0;
	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), 4.0f, radius * glm::cos(time));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program->SetUniformMat4f("u_Projection", this->m_ProjMat);
	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniform3f("u_CameraPos", cameraPos);

		// 2
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, this->width, this->height);

		// 3
	//glCullFace(GL_BACK);
		// 4
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->pass2SubroutineIndex);
	    // 5
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::rotate(model, time, glm::vec3(0.0f, 0.0f, 1.0f));

		m_VBA_Torus->Bind();
		m_Program->SetUniformMat4f("ShadowMatrix", this->m_BPV_Light * model);
		m_Program->SetUniformMat4f("u_Model", model);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountTorus * 3));
	}

	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		m_VBA_Plane->Bind();
		m_Program->SetUniformMat4f("ShadowMatrix", this->m_BPV_Light * model);
		m_Program->SetUniformMat4f("u_Model", model);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountPlane * 3));
	}
	
	


/*	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();*/
}

void core::Example_7_1_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_7_1_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_7_1_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_7_1_Window::OnScrollAction(double xoffset, double yoffset)
{
}
