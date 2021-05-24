#include "DrawOrderWindow.h"

#include "DrawOrderWindow.h"

#include "glm/gtc/random.hpp"
#include "../../../core/opengl/core/error/ErrorChecker.h"
#include "stb_image\stb_image.h"

#include <vector>


core::DrawOrderWindow::DrawOrderWindow(const int w, const int h) : Window(w, h, "DrawOrderWindow"),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
	VAO(0),
	time(0),
	count(6)
{
	std::cout << "DrawOrderWindow Constructor " << std::endl;

}

core::DrawOrderWindow::~DrawOrderWindow()
{
	delete m_RenderProg;

	delete m_Grid;
}


void core::DrawOrderWindow::Init()
{
	std::cout << "DrawOrderWindow Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);


	// Set up the VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	// Positions, Sizes, Colors
	
	float umax = 0xFFFFFFFF;
	//
/*	unsigned int um = umax;

	std::cout << std::endl << "umax << " << um << std::endl;
	*/

	float* vertData = new float[this->count * (2 + 2 + 4 + 1)]
	{
		-2.0f, 0.0f,   5.0f, 5.0f,   1.0f, 0.0f, 0.0f, 0.1f,  umax,
		-2.0f, 0.0f,  3.0f, 4.5f,   0.0f, 1.0f, 0.0f, 0.1f,  2,
		-1.0f, -2.0f,  6.0f, 5.5f,   0.0f, 1.0f, 0.0f, 0.1f,  0,


		 0.7f, 1.2f,   2.0f, 4.0f,   1.0f, 1.0f, 0.0f, 0.1f,  umax,
		 1.2f, 1.2f,   1.0f, 2.5f,   0.0f, 1.0f, 0.0f, 0.2f,  3,
		 0.7f, 0.7f,   0.6f, 0.5f,   0.0f, 1.0f, 0.0f, 0.2f,  3
		
	};

	unsigned int stride = 8 * sizeof(float) + sizeof(unsigned int);

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, this->count * (2 + 2 + 4 + 1) * sizeof(GLfloat), vertData, GL_DYNAMIC_COPY);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void *) (2 * sizeof(float)) );

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (const void*) (4 * sizeof(float)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (const void*) (8 * sizeof(float)));

	delete[] vertData;

	unsigned int* indData = new unsigned int[count]
	{
		0, 2, 1, 3, 4, 5
	};

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indData, GL_STATIC_DRAW);

	delete[] indData;

	// bind buffer of VBO as SSBO
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, VBO); // 0 - binding point

	

	// ssbo counters
	glGenBuffers(1, &SSBO_Counters);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBO_Counters); // 1 - binding point

	glBufferData(GL_SHADER_STORAGE_BUFFER, this->width * this->height * 4 * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);  // for my screen it is 17 920 000 bytes

	//glClearNamedBufferData(SSBO_Counters, GL_RGBA32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, clearVals);


	m_RenderProg = new Program("res/shaders/ui/draw-order", "VS.glsl", "FS.glsl", "GS.glsl");
	m_RenderProg->Bind();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	m_RenderProg->SetUniformMat4f("u_Projection", this->m_ProjMat);
	m_RenderProg->SetUniformMat4f("u_Model", model);


	this->pass1Id = glGetSubroutineIndex(
		((opengl::Bindable*) m_RenderProg)->GetId(),
		GL_FRAGMENT_SHADER,
		"pass1"
	);

	this->pass2Id = glGetSubroutineIndex(
		((opengl::Bindable*) m_RenderProg)->GetId(),
		GL_FRAGMENT_SHADER,
		"pass2"
	);


	// Set up the plane VAO
	{
	
		
		glGenVertexArrays(1, &planeVAO);
		glBindVertexArray(planeVAO);
	
		vertData = new float[this->count * (2 + 2 + 4 + 1)]
		{
			 0.0f, 0.0f,   2.0f, 2.0f,   1.0f, 0.0f, 0.0f, 1.0f,  0,
		};

		unsigned int stride = 8 * sizeof(float) + sizeof(unsigned int);

		glGenBuffers(1, &planeVBO);

		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, this->count * (2 + 2 + 4 + 1) * sizeof(GLfloat), vertData, GL_DYNAMIC_COPY);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(2 * sizeof(float)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (const void*)(4 * sizeof(float)));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (const void*)(8 * sizeof(float)));

		delete[] vertData;

	}

	




	glViewport(0, 0, this->width, this->height);


	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glEnable(GL_MULTISAMPLE);
	glEnable(GL_POLYGON_SMOOTH);


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


void core::DrawOrderWindow::Loop(float dt)
{
	time += dt;

	//std::cout << "FPS: " << (1 / dt) << std::endl;

	m_RenderProg->Bind();
	glBindVertexArray(VAO);

	time = 0;

	float radius = 5.0;
	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time * 0.4f), 0.0f, radius * glm::cos(time * 0.4f));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_RenderProg->SetUniformMat4f("u_View", m_ViewMat);

	// pass 1
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->pass1Id);
	glClearNamedBufferData(SSBO_Counters, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, clearVals);

	//std::cout << "Clear" << std::endl;

	glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, nullptr);

	//std::cout << "Full" << std::endl;

	//glMemoryBarrier( GL_ALL_BARRIER_BITS );


	// pass 2
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &this->pass2Id);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(planeVAO);

	m_RenderProg->SetUniformMat4f("u_View", glm::lookAt(glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	glDrawArrays(GL_POINTS, 0, 1);

	//glMemoryBarrier(GL_ALL_BARRIER_BITS);

	//////////
/*	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();
	*/
}

void core::DrawOrderWindow::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		

	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		

	}


	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::DrawOrderWindow::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::DrawOrderWindow::OnMouseButtonAction(int button, int action, int mods)
{
	double xpos, ypos;
	GetCursorPosition(&xpos, &ypos);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		

		if (xpos <= width && ypos <= height)
		{

			int _y = height - ypos;
			int _x = xpos;

			std::cout << "X = " << _x << " Y = " << _y << " GLFW_MOUSE_BUTTON_LEFT" << std::endl;

			int offset = _y * width * 16 + _x * 16;

			unsigned int out_data[4];

		//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Counters);
			glGetNamedBufferSubData(SSBO_Counters, offset, 4 * sizeof(unsigned int), out_data);


			std::cout << "out_data[0] = " << out_data[0] << std::endl;
			std::cout << "out_data[1] = " << out_data[1] << std::endl;
			std::cout << "out_data[2] = " << out_data[2] << std::endl;
			std::cout << "out_data[3] = " << out_data[3] << std::endl;
		}

	}

}

void core::DrawOrderWindow::OnScrollAction(double xoffset, double yoffset)
{
}

