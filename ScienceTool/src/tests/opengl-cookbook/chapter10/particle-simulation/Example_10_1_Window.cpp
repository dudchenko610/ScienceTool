#include "Example_10_1_Window.h"

#include "glm/gtc/random.hpp"
#include "../../../../core/opengl/core/error/ErrorChecker.h"
#include "stb_image\stb_image.h"

#include <vector>


core::Example_10_1_Window::Example_10_1_Window(const int w, const int h) : Window(w, h, "Example_10_1_Window"),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	nParticles(95, 95, 95),
	time(0.0f), deltaT(0.0f), speed(35.0f), angle(0.0f),
	bh1(5, 0, 0, 1), bh2(-5, 0, 0, 1)
{
	std::cout << "Example_10_1_Window Constructor " << std::endl;

	totalParticles = nParticles.x * nParticles.y * nParticles.z;
}

core::Example_10_1_Window::~Example_10_1_Window()
{
	delete m_RenderProg;
	delete m_ComputeProg;

	delete m_Grid;
}


void core::Example_10_1_Window::Init()
{
	std::cout << "Example_10_1_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	// Initial positions of the particles
	std::vector<GLfloat> initPos;
	std::vector<GLfloat> initVel(totalParticles * 4, 0.0f);

	{
		glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

		GLfloat dx = 2.0f / (nParticles.x - 1),
			dy = 2.0f / (nParticles.y - 1),
			dz = 2.0f / (nParticles.z - 1);

		// We want to center the particles at (0,0,0)
		glm::mat4 transf = glm::translate(glm::mat4(1.0f), glm::vec3(-1, -1, -1));

		for (int i = 0; i < nParticles.x; i++) {
			for (int j = 0; j < nParticles.y; j++) {
				for (int k = 0; k < nParticles.z; k++) {
					p.x = dx * i;
					p.y = dy * j;
					p.z = dz * k;
					p.w = 1.0f;
					p = transf * p;
					initPos.push_back(p.x);
					initPos.push_back(p.y);
					initPos.push_back(p.z);
					initPos.push_back(p.w);
				}
			}
		}
	}

	
	// We need buffers for position , and velocity.
	GLuint bufVBOs[2];

	glGenBuffers(2, bufVBOs);
	GLuint posBufVBO = bufVBOs[0];
	GLuint velBufVBO = bufVBOs[1];

	GLuint bufSize = totalParticles * 4 * sizeof(GLfloat);

	{
		// The buffers for positions
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBufVBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &initPos[0], GL_DYNAMIC_DRAW);

		// Velocities
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBufVBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &initVel[0], GL_DYNAMIC_COPY);
	}

	{
		// Set up the VAO
		glGenVertexArrays(1, &particlesVAO);
		glBindVertexArray(particlesVAO);

		glBindBuffer(GL_ARRAY_BUFFER, posBufVBO);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}


	{
		// Set up a buffer and a VAO for drawing the attractors (the "black holes")
		glGenBuffers(1, &bhBufVBO);
		glBindBuffer(GL_ARRAY_BUFFER, bhBufVBO);

		GLfloat data[] = { bh1.x, bh1.y, bh1.z, bh1.w, bh2.x, bh2.y, bh2.z, bh2.w };
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &bhVAO);
		glBindVertexArray(bhVAO);

		glBindBuffer(GL_ARRAY_BUFFER, bhBufVBO);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}


	m_RenderProg = new Program("res/shaders/opengl-cookbook/chapter10/ex_1", "ParticleSimulation_VS.glsl", "ParticleSimulation_FS.glsl");
	m_ComputeProg = new Program("res/shaders/opengl-cookbook/chapter10/ex_1", "ParticleSimulation_CS.glsl");
	
	m_RenderProg->Bind();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	m_RenderProg->SetUniformMat4f("u_Projection", this->m_ProjMat);
	m_RenderProg->SetUniformMat4f("u_Model", model);

	m_RenderProg->Unbind();


	glViewport(0, 0, this->width, this->height);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


void core::Example_10_1_Window::Loop(float dt)
{
	time += dt;
	
	std::cout << "FPS: " << (1 / dt) << std::endl;

//	angle += speed * deltaT;
	if (angle > 360.0f)
	{
		angle -= 360.0f;
	}

	// Rotate the attractors ("black holes")
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 0, 1));
	glm::vec3 att1 = glm::vec3(rot * bh1);
	glm::vec3 att2 = glm::vec3(rot * bh2);

	// Execute the compute shader
	m_ComputeProg->Bind();
	m_ComputeProg->SetUniform3f("BlackHolePos1", att1);
	m_ComputeProg->SetUniform3f("BlackHolePos2", att2);

	glDispatchCompute(totalParticles / 1000, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// Draw the scene
	m_RenderProg->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float radius = 10.0;
	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time * 0.4f), 1.0f, radius * glm::cos(time * 0.4f));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_RenderProg->SetUniformMat4f("u_View", m_ViewMat);

	// Draw the particles
	glPointSize(1.0f);
	m_RenderProg->SetUniform4f("Color", glm::vec4(1.0f, 0, 0, 0.2f));

	glBindVertexArray(particlesVAO);
	glDrawArrays(GL_POINTS, 0, totalParticles);
	glBindVertexArray(0);

	// Draw the attractors
	glPointSize(5.0f);
	GLfloat data[] = { att1.x, att1.y, att1.z, 1.0f, att2.x, att2.y, att2.z, 1.0f };

	glBindBuffer(GL_ARRAY_BUFFER, bhBufVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(GLfloat), data);

	m_RenderProg->SetUniform4f("Color", glm::vec4(1, 1, 0, 1.0f));
	glBindVertexArray(bhVAO);
	glDrawArrays(GL_POINTS, 0, 2);
	glBindVertexArray(0);


	//////////
	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();

}

void core::Example_10_1_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_10_1_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_10_1_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_10_1_Window::OnScrollAction(double xoffset, double yoffset)
{
}
