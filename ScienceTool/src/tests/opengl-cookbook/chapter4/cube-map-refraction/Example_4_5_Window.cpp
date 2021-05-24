#include "Example_4_5_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

#include "stb_image/stb_image.h"

core::Example_4_5_Window::Example_4_5_Window(const int w, const int h, const char* l) : Window(w, h, l),

	//	m_ProjMat(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -0.1f, 10.0f)),
	m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 200.0f)),
	m_ViewMat(glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
	m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0))),

	trianglesCount(0)

{
	std::cout << "Example_4_5_Window Constructor " << std::endl;

}

core::Example_4_5_Window::~Example_4_5_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;

	delete m_Grid;

	delete m_VBA_Cube;
	delete m_VBO_Cube;
	delete m_VBL_Cube;

}

void core::Example_4_5_Window::Init()
{
	std::cout << "Example_4_5_Window Init" << std::endl;
	using namespace opengl;

	m_Grid = new Grid(true, false, false);

	m_Program = new Program("res/shaders/opengl-cookbook/chapter4/ex_5/CubeMapRefraction.shader");

	parser::Vertices* vertsCube = parser::obj::ObjParser::Parse("res/obj/cube_large.objj");
	this->trianglesCountCube = vertsCube->GetTrianglesCount();

	m_VBA_Cube = new VertexArray();
	m_VBO_Cube = new VertexBuffer(vertsCube->GetData(), vertsCube->GetSize());
	m_VBL_Cube = new VertexBufferLayout();

	delete vertsCube;

	m_VBL_Cube->Push<float>(3);
	m_VBL_Cube->Push<float>(3);


	m_VBA_Cube->AddBuffer(*m_VBO_Cube, *m_VBL_Cube);

	// object

	parser::Vertices* verts = parser::obj::ObjParser::Parse("res/obj/torus_knot.objj");
	this->trianglesCount = verts->GetTrianglesCount();

	m_VBA = new VertexArray();
	m_VBO = new VertexBuffer(verts->GetData(), verts->GetSize());
	m_VBL = new VertexBufferLayout();

	delete verts;

	m_VBL->Push<float>(3);
	m_VBL->Push<float>(3);

	m_VBA->AddBuffer(*m_VBO, *m_VBL);


	m_Program->Bind();
	m_Program->SetUniformMat4f("u_Projection", m_ProjMat);
	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniformMat4f("u_Model", m_ModelMat);



	m_Program->SetUniform1f("Material.Eta", 0.9f);
	m_Program->SetUniform1f("Material.ReflectionFactor", 0.1f);


	////////////

	glActiveTexture(GL_TEXTURE0);

	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	const char* suffixes[] = { "posx", "negx", "posy",
		"negy", "posz", "negz" };

	unsigned int targets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	int width, height, bpp;
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, 256, 256); // allocate memory

	std::string baseFileName = "res/textures/cube-map/";

	for (int i = 0; i < 6; i++)
	{
		std::string texName = std::string(baseFileName) + "night_" + suffixes[i] + ".png";
		unsigned char* data = stbi_load(texName.c_str(), &width, &height, &bpp, 4);

		glTexSubImage2D(targets[i], 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

		if (data)
		{
			stbi_image_free(data);
		}

	}

	// Typical cube map settings
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	m_Program->Unbind();
	m_VBA->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void core::Example_4_5_Window::Loop(float dt)
{
	time += dt;

//	std::cout << "FPS" << (1 / dt) << std::endl;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	m_Program->Bind();

	float radius = 2.5f;
	glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), radius * glm::cos(time), radius * glm::cos(time));
	m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // eye lookAt up-vector

	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniform3f("u_CameraPos", cameraPos);

	// draw cube
	m_Program->SetUniform1i("DrawSkyBox", true);
	m_VBA_Cube->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCountCube * 3));


	// draw object
	m_Program->SetUniform1i("DrawSkyBox", false);
	m_VBA->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, this->trianglesCount * 3));



	m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
	m_Grid->Unbind();
}

void core::Example_4_5_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_4_5_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_4_5_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_4_5_Window::OnScrollAction(double xoffset, double yoffset)
{
}
