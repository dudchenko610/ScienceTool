#include "Example_9_3_Window.h"

#include "glm/gtc/random.hpp"
#include "../../../../core/opengl/core/error/ErrorChecker.h"
#include <stb_image\stb_image.h>

core::Example_9_3_Window::Example_9_3_Window(const int w, const int h) : Window(w, h, "Example_9_3_Window"),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	nParticles(8000),
	drawBuf(1)
{
	std::cout << "Example_9_3_Window Constructor " << std::endl;

}

core::Example_9_3_Window::~Example_9_3_Window()
{
	delete m_Program;
}

float core::Example_9_3_Window::randFloat()
{
	return ((float)rand() / RAND_MAX);
}

void core::Example_9_3_Window::Init()
{
	std::cout << "Example_9_3_Window Init" << std::endl;
	using namespace opengl;

	/*
		The order of the names in this
		list corresponds to the indices of the feedback buffers. In this case, Position corresponds to
		index zero, Velocity to index one, and StartTime to index two. Check back to the previous
		code that creates our feedback buffer objects (the glBindBufferBase calls) to verify that
		this is indeed the case.
	*/


	// program initialization
	{
		m_Program = new Program("res/shaders/opengl-cookbook/chapter9/ex_3", "AnimatedFountain_VS.glsl", "AnimatedFountain_FS.glsl", 0);
		programId = ((Bindable*)m_Program)->GetId();

		const char* outputNames[] = { "Position", "Velocity", "StartTime" };
		glTransformFeedbackVaryings(programId, 3, outputNames, GL_SEPARATE_ATTRIBS);

		m_Program->LinkAndValidate();
		m_Program->Bind();

	}

	// subroutines initialization
	{
		renderSubroutineId = glGetSubroutineIndex(programId, GL_VERTEX_SHADER, "render");
		updateSubroutineId = glGetSubroutineIndex(programId, GL_VERTEX_SHADER, "update");
	}

	// generate buffers 
	{
		// Generate the buffers
		glGenBuffers(2, posVBO);       // position buffers
		glGenBuffers(2, velVBO);       // velocity buffers
		glGenBuffers(2, startTimeVBO); // Start time buffers
		glGenBuffers(1, &initVelVBO);     // Initial velocity buffer (never changes, only need one)
	
		// Allocate space for all buffers
		int size = nParticles * 3 * sizeof(GLfloat);

		glBindBuffer(GL_ARRAY_BUFFER, posVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);

		glBindBuffer(GL_ARRAY_BUFFER, velVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);

		glBindBuffer(GL_ARRAY_BUFFER, startTimeVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_DYNAMIC_COPY);



		glBindBuffer(GL_ARRAY_BUFFER, posVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);

		glBindBuffer(GL_ARRAY_BUFFER, velVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);

		glBindBuffer(GL_ARRAY_BUFFER, startTimeVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_DYNAMIC_COPY);



		glBindBuffer(GL_ARRAY_BUFFER, initVelVBO);
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);




		// Fill the first position buffer with zeroes
		GLfloat* data = new GLfloat[nParticles * 3];

		for (int i = 0; i < nParticles * 3; i++) 
		{
			data[i] = 0.0f;
		}

		glBindBuffer(GL_ARRAY_BUFFER, posVBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

		// Fill the first velocity buffer with random velocities

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

		glBindBuffer(GL_ARRAY_BUFFER, velVBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

		glBindBuffer(GL_ARRAY_BUFFER, initVelVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);


		// Fill the first start time buffer
		delete[] data;
		data = new GLfloat[nParticles];
		float time = 0.0f;
		float rate = 0.001f;

		for (int i = 0; i < nParticles; i++) {
			data[i] = time;
			time += rate;
		}

		glBindBuffer(GL_ARRAY_BUFFER, startTimeVBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
		delete[] data;

		// Create vertex arrays for each set of buffers
		glGenVertexArrays(2, particleVAO);


		{
			// Set up particle array 0
			glBindVertexArray(particleVAO[0]);
			glBindBuffer(GL_ARRAY_BUFFER, posVBO[0]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, velVBO[0]);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, startTimeVBO[0]);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, initVelVBO);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(3);
		}
	
		{
			// Set up particle array 1
			glBindVertexArray(particleVAO[1]);
			glBindBuffer(GL_ARRAY_BUFFER, posVBO[1]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, velVBO[1]);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, startTimeVBO[1]);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, initVelVBO);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(3);

			glBindVertexArray(0);
		}


		// Setup the feedback objects
		glGenTransformFeedbacks(2, feedback);

		// Transform feedback 0
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posVBO[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velVBO[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, startTimeVBO[0]);

		// Transform feedback 1
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posVBO[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velVBO[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, startTimeVBO[1]);

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

		GLint value;
		glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &value);
		printf("MAX_TRANSFORM_FEEDBACK_BUFFERS = %d\n", value);
		

	}



	m_Program->SetUniformMat4f("u_Projection", this->m_ProjMat);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Program->SetUniformMat4f("u_Model", model);

	m_Program->SetUniform1f("ParticleLifetime", 3.5f);
	m_Program->SetUniform3f("Accel", glm::vec3(0.0f, -0.05f, 0.0f));



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


	m_Program->Unbind();

	glViewport(0, 0, this->width, this->height);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(10.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


void core::Example_9_3_Window::Loop(float dt)
{
	time += dt;

	m_Program->Bind();
	m_Program->SetUniform1f("Time", time);
	m_Program->SetUniform1f("H", dt);


	// Update pass
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &updateSubroutineId);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);

	{
		glBeginTransformFeedback(GL_POINTS);
		glBindVertexArray(particleVAO[1 - drawBuf]);
		glDrawArrays(GL_POINTS, 0, nParticles);
		glEndTransformFeedback();
	}

	// Render pass
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &renderSubroutineId);

	glDisable(GL_RASTERIZER_DISCARD);
	glClear(GL_COLOR_BUFFER_BIT);

	float radius = 3.0;
	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time * 0.4f), 1.0f, radius * glm::cos(time * 0.4f));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Program->SetUniformMat4f("u_View", m_ViewMat);

	{
		glBindVertexArray(particleVAO[drawBuf]);
		glDrawTransformFeedback(GL_POINTS, feedback[drawBuf]);
	}


	// Swap buffers
	drawBuf = 1 - drawBuf;


}

void core::Example_9_3_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_9_3_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_9_3_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_9_3_Window::OnScrollAction(double xoffset, double yoffset)
{
}
