#include "Example_9_2_Window.h"

#include "glm/gtc/random.hpp"
#include "../../../../core/opengl/core/error/ErrorChecker.h"
#include <stb_image\stb_image.h>

core::Example_9_2_Window::Example_9_2_Window(const int w, const int h) : Window(w, h, "Example_9_2_Window"),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	nParticles(8000)
{
	std::cout << "Example_9_2_Window Constructor " << std::endl;

}

core::Example_9_2_Window::~Example_9_2_Window()
{
	delete m_Program;

	delete m_VBA;

	delete m_VBO_Pos;
	delete m_VBL_Pos;

	delete m_VBO_Time;
	delete m_VBL_Time;


	delete m_Grid;
}

float core::Example_9_2_Window::randFloat()
{
	return ((float)rand() / RAND_MAX);
}

void core::Example_9_2_Window::Init()
{
	std::cout << "Example_9_2_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	m_Program = new Program("res/shaders/opengl-cookbook/chapter9/ex_2", "AnimatedFountain_VS.glsl", "AnimatedFountain_FS.glsl");

	{

		m_VBA = new VertexArray();
		m_VBA->Bind();

		GLfloat* data = new GLfloat[nParticles * 3];

		glm::vec3 v(0.0f);
		float velocity, theta, phi;

		for (unsigned int i = 0; i < nParticles; i++) {

			// Pick the direction of the velocity
			theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, randFloat());
			phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());

			v.x = sinf(theta) * cosf(phi);
			v.y = cosf(theta);
			v.z = sinf(theta) * sinf(phi);

			// Scale to set the magnitude of the velocity (speed)
			velocity = glm::mix(1.25f, 1.5f, randFloat());
			v = v * velocity;

			data[3 * i] = v.x;
			data[3 * i + 1] = v.y;
			data[3 * i + 2] = v.z;
		}


		m_VBO_Pos = new VertexBuffer(data, nParticles * 3 * sizeof(float));
	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		
		//m_VBL_Pos = new VertexBufferLayout();
	//	m_VBL_Pos->Push<float>(3);

		delete[] data;

		data = new GLfloat[nParticles];
		float time = 0.0f, rate = 0.00075f;

		for (unsigned int i = 0; i < nParticles; i++) {
			data[i] = time;
			time += rate;
		}

		m_VBO_Time = new VertexBuffer(data, nParticles * sizeof(float));

		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

	//	m_VBL_Time = new VertexBufferLayout();
		//m_VBL_Time->Push<float>(1);

		delete[] data;


	//	m_VBA->AddBuffer(*m_VBO_Pos, *m_VBL_Pos);
	//	m_VBA->AddBuffer(*m_VBO_Time, *m_VBL_Time);

	}



	m_Program->Bind();

	m_Program->SetUniformMat4f("u_Projection", this->m_ProjMat);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Program->SetUniformMat4f("u_Model", model);

	m_Program->SetUniform1f("ParticleLifetime", 9.5f);
	

	// gen texture

	int width, height, bpp;
	unsigned int texId;
	glGenTextures(1, &texId);

	// Copy brick texture to OpenGL
	unsigned char* data = stbi_load("res/textures/image1.png", &width, &height, &bpp, 4);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height); // allocate memory
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data); // copy data

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (data)
	{
		stbi_image_free(data);
	}


	m_Program->Unbind();
	m_VBA->Unbind();

	glViewport(0, 0, this->width, this->height);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(10.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


void core::Example_9_2_Window::Loop(float dt)
{
	time += dt;

	m_Program->Bind();

	m_Program->SetUniform1f("Time", time);

	glClear(GL_COLOR_BUFFER_BIT);

	float radius = 3.0; 
	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time * 0.4f), 1.0f, radius * glm::cos(time * 0.4f));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program->SetUniformMat4f("u_View", m_ViewMat);


	m_VBA->Bind();

	GLCall(glDrawArrays(GL_POINTS, 0, this->nParticles));

	/*
	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();*/
}

void core::Example_9_2_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_9_2_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_9_2_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_9_2_Window::OnScrollAction(double xoffset, double yoffset)
{
}
