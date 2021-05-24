#include "Example_10_2_Window.h"

#include "glm/gtc/random.hpp"
#include "../../../../core/opengl/core/error/ErrorChecker.h"
#include "stb_image\stb_image.h"

#include <vector>

#define PRIM_RESTART 0xffffff

core::Example_10_2_Window::Example_10_2_Window(const int w, const int h) : Window(w, h, "Example_10_2_Window"),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	clothVAO(0),
	numElements(0),
	nParticles(20, 20), 
	clothSize(4.0f, 3.0f),
	time(0.0f), deltaT(0.0f), 
	speed(200.0f), 
	readBuf(0)
{
	std::cout << "Example_10_2_Window Constructor " << std::endl;
	
}

core::Example_10_2_Window::~Example_10_2_Window()
{
	delete m_RenderProg;
	delete m_ComputeProg;
	delete m_ComputeNormProg;

	delete m_Grid;
}


void core::Example_10_2_Window::Init()
{
	std::cout << "Example_10_2_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(PRIM_RESTART);


	// Initial transform
	glm::mat4 transf = glm::translate(glm::mat4(1.0), glm::vec3(-2.0f, clothSize.y, 0));
	transf = glm::rotate(transf, glm::radians(-80.0f), glm::vec3(1, 0, 0));
	transf = glm::translate(transf, glm::vec3(0, -clothSize.y, 0));

	// Initial positions of the particles
	std::vector<GLfloat> initPos;
	std::vector<GLfloat> initVel(nParticles.x * nParticles.y * 4, 0.0f);
	std::vector<float> initTc;

	{
		float dx = clothSize.x / (nParticles.x - 1);
		float dy = clothSize.y / (nParticles.y - 1);
		float ds = 1.0f / (nParticles.x - 1);
		float dt = 1.0f / (nParticles.y - 1);

		glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

		for (int i = 0; i < nParticles.y; i++) {
			for (int j = 0; j < nParticles.x; j++) {
				p.x = dx * j;
				p.y = dy * i;
				p.z = 0.0f;
				p = transf * p;
				initPos.push_back(p.x);
				initPos.push_back(p.y);
				initPos.push_back(p.z);
				initPos.push_back(1.0f);

				initTc.push_back(ds * j);
				initTc.push_back(dt * i);
			}
		}
	}

	

	// Every row is one triangle strip
	std::vector<GLuint> el;

	{
		for (int row = 0; row < nParticles.y - 1; row++) {
			for (int col = 0; col < nParticles.x; col++) {
				el.push_back((row + 1) * nParticles.x + (col));
				el.push_back((row)*nParticles.x + (col));
			}
			el.push_back(PRIM_RESTART);
		}
	}

	

	// We need buffers for position (2), element index,
	// velocity (2), normal, and texture coordinates.
	GLuint bufVBOs[7];
	glGenBuffers(7, bufVBOs);
	posBufsVBO[0] = bufVBOs[0];
	posBufsVBO[1] = bufVBOs[1];
	velBufsVBO[0] = bufVBOs[2];
	velBufsVBO[1] = bufVBOs[3];
	normBufVBO    = bufVBOs[4];
	elBufVBO      = bufVBOs[5];
	tcBufVBO      = bufVBOs[6];

	GLuint parts = nParticles.x * nParticles.y;

	{

		// The buffers for positions
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBufsVBO[0]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, parts * 4 * sizeof(GLfloat), &initPos[0], GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, posBufsVBO[1]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, parts * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

		// Velocities
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, velBufsVBO[0]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, parts * 4 * sizeof(GLfloat), &initVel[0], GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, velBufsVBO[1]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, parts * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_COPY);

		// Normal buffer
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, normBufVBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, parts * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_COPY);

		// Element indicies
		glBindBuffer(GL_ARRAY_BUFFER, elBufVBO);
		glBufferData(GL_ARRAY_BUFFER, el.size() * sizeof(GLuint), &el[0], GL_DYNAMIC_COPY);

		// Texture coordinates
		glBindBuffer(GL_ARRAY_BUFFER, tcBufVBO);
		glBufferData(GL_ARRAY_BUFFER, initTc.size() * sizeof(GLfloat), &initTc[0], GL_STATIC_DRAW);
	}

	numElements = GLuint(el.size());

	{
		// Set up the VAO
		glGenVertexArrays(1, &clothVAO);
		glBindVertexArray(clothVAO);

		glBindBuffer(GL_ARRAY_BUFFER, posBufsVBO[0]);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, normBufVBO);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, tcBufVBO);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elBufVBO);
		glBindVertexArray(0);
	}

	m_RenderProg = new Program("res/shaders/opengl-cookbook/chapter10/ex_2", "ClothSimulation_VS.glsl", "ClothSimulation_FS.glsl");
	m_ComputeProg = new Program("res/shaders/opengl-cookbook/chapter10/ex_2", "ClothSimulation_CS.glsl");
	m_ComputeNormProg = new Program("res/shaders/opengl-cookbook/chapter10/ex_2", "ClothSimulatioNormals_CS.glsl");

	m_RenderProg->Bind();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	m_RenderProg->SetUniformMat4f("u_Projection", this->m_ProjMat);
	m_RenderProg->SetUniformMat4f("u_Model", model);

	m_RenderProg->SetUniform4f("LightPosition", glm::vec4(0.0f, 1.0f, 2.0f, 1.0f));
	m_RenderProg->SetUniform3f("LightIntensity", glm::vec3(1.0f));
	m_RenderProg->SetUniform3f("Kd", glm::vec3(0.8f));
	m_RenderProg->SetUniform3f("Ka", glm::vec3(0.2f));
	m_RenderProg->SetUniform3f("Ks", glm::vec3(0.2f));
	m_RenderProg->SetUniform1f("Shininess", 10.0f);

	{
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
	}


	m_RenderProg->Unbind();

	

	m_ComputeProg->Bind();
	float dx = clothSize.x / (nParticles.x - 1);
	float dy = clothSize.y / (nParticles.y - 1);

	m_ComputeProg->SetUniform1f("RestLengthHoriz", dx);
	m_ComputeProg->SetUniform1f("RestLengthVert", dy);
	m_ComputeProg->SetUniform1f("RestLengthDiag", sqrtf(dx * dx + dy * dy));



	
	glViewport(0, 0, this->width, this->height);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


void core::Example_10_2_Window::Loop(float dt)
{
	time += dt;

	std::cout << "FPS: " << (1 / dt) << std::endl;

	
	m_ComputeProg->Bind();

	for (int i = 0; i < 300; i++) {
		glDispatchCompute(nParticles.x / 10, nParticles.y / 10, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// Swap buffers
		readBuf = 1 - readBuf;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBufsVBO[readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, posBufsVBO[1 - readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, velBufsVBO[readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, velBufsVBO[1 - readBuf]);
	}

	// Compute the normals
	m_ComputeNormProg->Bind();
	glDispatchCompute(nParticles.x / 10, nParticles.y / 10, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// Now draw the scene
	m_RenderProg->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float radius = 6.0;
	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time * 0.4f), 1.0f, radius * glm::cos(time * 0.4f));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_RenderProg->SetUniformMat4f("u_View", m_ViewMat);

	// Draw the cloth
	glBindVertexArray(clothVAO);
	glDrawElements(GL_TRIANGLE_STRIP, numElements, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);



	//////////
	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();

}

void core::Example_10_2_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_10_2_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_10_2_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_10_2_Window::OnScrollAction(double xoffset, double yoffset)
{
}
