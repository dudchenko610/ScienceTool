#include "Example_5_6_Window.h"

#include <iostream>
#include <vector>

#include "../../../../core/opengl/core/error/ErrorChecker.h"
#include "obj-parser/ObjParser.h"

/*

Of course, we are ignoring the fact that, in reality, the intensity
of the light decreases with the square of the distance from the
source. However, it is not uncommon to ignore this aspect for
directional light sources.

*/

core::Example_5_6_Window::Example_5_6_Window(const int w, const int h, const char* l) : Window(w, h, l),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0)))
{
	std::cout << "Example_5_6_Window Constructor " << std::endl;

}

core::Example_5_6_Window::~Example_5_6_Window()
{
	delete m_Program_Obj;

	delete m_VBA_Torus;
	delete m_VBO_Torus;
	delete m_VBL_Torus;

	delete m_VBA_Sphere;
	delete m_VBO_Sphere;
	delete m_VBL_Sphere;

	delete m_VBA_Plane;
	delete m_VBO_Plane;
	delete m_VBL_Plane;

	delete m_Grid;
}

void core::Example_5_6_Window::Init()
{
	std::cout << "Example_5_6_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);


	{
		m_Program_Obj = new Program("res/shaders/opengl-cookbook/chapter5/ex_6/OrderIndependentTransparency.shader");


		{
			parser::Vertices* vertsObj = parser::obj::ObjParser::Parse("res/obj/torus_knot.objj");
			this->trianglesCountTorus = vertsObj->GetTrianglesCount();

			m_VBA_Torus = new VertexArray();
			m_VBO_Torus = new VertexBuffer(vertsObj->GetData(), vertsObj->GetSize());
			m_VBL_Torus = new VertexBufferLayout();

			delete vertsObj;

			m_VBL_Torus->Push<float>(3);
			m_VBL_Torus->Push<float>(3);

			m_VBA_Torus->AddBuffer(*m_VBO_Torus, *m_VBL_Torus);
		}


		{
			parser::Vertices* vertsSphere = parser::obj::ObjParser::Parse("res/obj/sphere2.objj");
			this->trianglesCountSphere = vertsSphere->GetTrianglesCount();

			m_VBA_Sphere = new VertexArray();
			m_VBO_Sphere = new VertexBuffer(vertsSphere->GetData(), vertsSphere->GetSize());
			m_VBL_Sphere = new VertexBufferLayout();

			delete vertsSphere;

			m_VBL_Sphere->Push<float>(3);
			m_VBL_Sphere->Push<float>(3);
			m_VBA_Sphere->AddBuffer(*m_VBO_Sphere, *m_VBL_Sphere);
		}


		m_Program_Obj->Bind();

		m_Program_Obj->SetUniformMat4f("u_Projection", this->m_ProjMat);
		m_Program_Obj->SetUniformMat4f("u_Model", this->m_ModelMat);


		GLCall(this->_pass1_SubroutineIndex = glGetSubroutineIndex(
			((opengl::Bindable*) m_Program_Obj)->GetId(),
			GL_FRAGMENT_SHADER,
			"pass1"
		));

	

		this->_pass2_SubroutineIndex = glGetSubroutineIndex(
			((opengl::Bindable*) m_Program_Obj)->GetId(),
			GL_FRAGMENT_SHADER,
			"pass2"
		);

		struct LightInfo {
			glm::vec3 position;
			glm::vec3 Intensity;
		};

		struct MaterialInfo {
			glm::vec4 Ka;		 // ambient reflectivity
			glm::vec3 Kd;		 // diffuse reflectivity
			glm::vec3 Ks;		 // specular reflectivity
			float Shininess; // specular shininess factor
		};

		float radius = 3.0f;

		m_Program_Obj->SetUniform3f("lights[0].Position", glm::vec3(-radius, 0.0f, 0.0f));
		m_Program_Obj->SetUniform3f("lights[0].Intensity", glm::vec3(0.0f, 0.0f, 1.0f));
		
		m_Program_Obj->SetUniform3f("lights[1].Position", glm::vec3(radius, 0.0f, 0.0f));
		m_Program_Obj->SetUniform3f("lights[1].Intensity", glm::vec3(1.0f, 0.0f, 0.0f));
		
		m_Program_Obj->SetUniform3f("lights[2].Position", glm::vec3(0.0, 0.0f, radius));
		m_Program_Obj->SetUniform3f("lights[2].Intensity", glm::vec3(0.0f, 0.0f, 0.0f));
		
		m_Program_Obj->SetUniform3f("lights[3].Position", glm::vec3(0.0, 0.0f, -radius));
		m_Program_Obj->SetUniform3f("lights[3].Intensity", glm::vec3(0.0f, 0.0f, 0.0f));
		
		m_Program_Obj->SetUniform3f("lights[4].Position", glm::vec3(0.0, radius, 0.0f));
		m_Program_Obj->SetUniform3f("lights[4].Intensity", glm::vec3(0.0f, 0.0f, 0.0f));

		MaterialInfo materialInfo;
		materialInfo.Ka = glm::vec4(0.1f, 0.1f, 0.1f, 0.1f);
		materialInfo.Kd = glm::vec3(0.3f, 0.3f, 0.3f);
		materialInfo.Ks = glm::vec3(0.9f, 0.9f, 0.9f);
		materialInfo.Shininess = 8;

		m_Program_Obj->SetUniform1f("Shininess", materialInfo.Shininess);
		m_Program_Obj->SetUniform4f("Ka", materialInfo.Ka);
		m_Program_Obj->SetUniform3f("Kd", materialInfo.Kd);
		m_Program_Obj->SetUniform3f("Ks", materialInfo.Ks);


	}


	// plane parsing and binding
	{
		parser::Vertices* vertsPlane = parser::obj::ObjParser::Parse("res/obj/plane_.objj");
		this->trianglesCountPlane = vertsPlane->GetTrianglesCount();

		m_VBA_Plane = new VertexArray();
		m_VBO_Plane = new VertexBuffer(vertsPlane->GetData(), vertsPlane->GetSize());
		m_VBL_Plane = new VertexBufferLayout();

		delete vertsPlane;

		m_VBL_Plane->Push<float>(3);
		m_VBL_Plane->Push<float>(3);

		m_VBA_Plane->AddBuffer(*m_VBO_Plane, *m_VBL_Plane);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	// set up a buffer for our atomic counter
	
	glGenBuffers(1, &this->counterBufferId);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, this->counterBufferId);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW); // allocate memory

	// create a buffer for our linked list storage
	unsigned int maxNodes = 20 * width * height;
	unsigned int nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint);

	unsigned int linkedListStorageId;
	glGenBuffers(1, &linkedListStorageId);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, linkedListStorageId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, maxNodes * nodeSize, NULL, GL_DYNAMIC_DRAW); // allocate memory

	m_Program_Obj->Bind();
	m_Program_Obj->SetUniform1ui("MaxNodes", maxNodes);
	m_Program_Obj->Unbind();

	// create a texture to hold the list head pointers. We'll  
	// use 32-bit unsigned integers, and bind it ti image unit 0
	
	glGenTextures(1, &this->headPtrTexId);
	glBindTexture(GL_TEXTURE_2D, this->headPtrTexId);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, width, height);
	glBindImageTexture(0, headPtrTexId, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

	// After we render each frame we need to clear the texture by setting 
	// all texels to a value of 0xffffffff - end of linked list
	std::vector<GLuint> headPtrClean(width * height, 0xffffffff);
	
	glGenBuffers(1, &this->clearBufId);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, this->clearBufId);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, headPtrClean.size() * sizeof(GLuint), &headPtrClean[0], GL_STATIC_COPY);

}

void core::Example_5_6_Window::Loop(float dt)
{

	time += dt;

	std::cout << "FPS = " << 1 / dt << std::endl;

	// Before the first pass, we want to clear our buffers to 
	// default values(that is, empty lists), and to reset our
	// atomic counter buffer to zero.

	GLuint zero = 0;
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterBufferId);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, this->clearBufId);
	glBindTexture(GL_TEXTURE_2D, this->headPtrTexId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);

	
	// compute camera position
	float radius = 2.5f;
	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), radius * glm::cos(time), radius * glm::cos(time));
	glm::mat4 viewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program_Obj->Bind();

	m_Program_Obj->SetUniformMat4f("u_Projection", this->m_ProjMat);
	m_Program_Obj->SetUniformMat4f("u_View", viewMat);
	m_Program_Obj->SetUniform3f("u_CameraPos", cameraPos);

	
	
	// PASS 1
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass1_SubroutineIndex);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_FALSE);


	if (this->isCorrectOrder)
	{
		m_VBA_Torus->Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountTorus * 3));

		m_VBA_Sphere->Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountSphere * 3));
	}
	else
	{
		m_VBA_Sphere->Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountSphere * 3));

		m_VBA_Torus->Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountTorus * 3));
	}
	
	// PASS2
	glMemoryBarrier( GL_ALL_BARRIER_BITS );
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->_pass2_SubroutineIndex);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	m_Program_Obj->SetUniformMat4f("u_Projection", glm::mat4(1.0f));
	m_Program_Obj->SetUniformMat4f("u_View", glm::mat4(1.0f));
	m_Program_Obj->SetUniformMat4f("u_Model", glm::mat4(1.0f));

	
	m_VBA_Plane->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountPlane * 3));

	
	/*
	m_Grid->BindAndDraw(this->m_ProjMat, viewMat);
	m_Grid->Unbind();*/
}

void core::Example_5_6_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		this->isCorrectOrder = !this->isCorrectOrder;
	}

}

void core::Example_5_6_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_5_6_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_5_6_Window::OnScrollAction(double xoffset, double yoffset)
{
}
