#include "Example_6_5_Window.h"

#include <iostream>
#include "../../../../core/opengl/core/error/ErrorChecker.h"

#include "obj-parser/ObjParser.h"

#include "TeapotData.h"

/*

This shader can be dropped in and applied to any mesh without any modification to the
OpenGL side of the application. It can be useful when debugging mesh issues or when
implementing a mesh modeling program.

*/


core::Example_6_5_Window::Example_6_5_Window(const int w, const int h) : Window(w, h, "Example_6_5_Window"),

    m_ProjMat(glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
    m_ViewMat(glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
    m_ModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)))

{
	std::cout << "Example_6_5_Window Constructor " << std::endl;

	float w2 = w / 2.0f;
	float h2 = h / 2.0f;

	this->m_ViewportMat = glm::mat4(
		glm::vec4(w2 + 0, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, h2 + 0, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
		glm::vec4(w2 + 0, h2 + 0, 0.0f, 1.0f));

}

core::Example_6_5_Window::~Example_6_5_Window()
{
	delete m_Program;
	delete m_VBA;
	delete m_VBO;
	delete m_VBA;

}

void core::Example_6_5_Window::Init()
{
	std::cout << "Example_6_5_Window Init" << std::endl;
	using namespace opengl;

    m_Grid = new Grid(true, false, false);

	glPatchParameteri(GL_PATCH_VERTICES, 16);

    std::vector<GLfloat> pts(32 * 16 * 3);
    generatePatches(pts);


	m_Program = new Program("res/shaders/opengl-cookbook/chapter6/ex_5", "TessSurface_VS.glsl", "TessSurface_FS.glsl", "TessSurface_GS.glsl", "TessSurface_TCS.glsl", "TessSurface_TES.glsl");

	m_VBA = new VertexArray();
	m_VBO = new VertexBuffer(pts.data(), pts.size() * sizeof(float));
	m_VBL = new VertexBufferLayout();


	m_VBL->Push<float>(3);

	m_VBA->AddBuffer(*m_VBO, *m_VBL);


	m_Program->Bind();
	m_Program->SetUniformMat4f("u_Projection", m_ProjMat);
	m_Program->SetUniformMat4f("u_View", m_ViewMat);
	m_Program->SetUniformMat4f("u_Model", m_ModelMat);
	m_Program->SetUniformMat4f("ViewportMatrix", m_ViewportMat);

	m_Program->SetUniform1i("TessLevel", 20);

    m_Program->SetUniform1f("LineWidth", 0.1f);
    m_Program->SetUniform4f("LineColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));


	struct LightInfo {
		glm::vec3 position;
		glm::vec3 Intensity;
	};

	struct MaterialInfo {
		glm::vec3 Ka;		 // ambient reflectivity
		glm::vec3 Kd;		 // diffuse reflectivity
		glm::vec3 Ks;		 // specular reflectivity
		float Shininess; // specular shininess factor
	};

	float radius = 4.0f;

	m_Program->SetUniform3f("lights[0].Position", glm::vec3(0.0, -radius, 0.0f));
	m_Program->SetUniform3f("lights[0].Intensity", glm::vec3(0.0f, 0.0f, 1.0f));

	m_Program->SetUniform3f("lights[1].Position", glm::vec3(0.0f, radius, 0.0f));
	m_Program->SetUniform3f("lights[1].Intensity", glm::vec3(1.0f, 0.0f, 0.0f));

	MaterialInfo materialInfo;
	materialInfo.Ka = glm::vec3(0.1f, 0.1f, 0.1f);
	materialInfo.Kd = glm::vec3(0.3f, 0.3f, 0.3f);
	materialInfo.Ks = glm::vec3(0.9f, 0.9f, 0.9f);
	materialInfo.Shininess = 8;

	m_Program->SetUniform1f("Shininess", materialInfo.Shininess);
	m_Program->SetUniform3f("Ka", materialInfo.Ka);
	m_Program->SetUniform3f("Kd", materialInfo.Kd);
	m_Program->SetUniform3f("Ks", materialInfo.Ks);


	m_Program->Unbind();
	m_VBA->Unbind();


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void core::Example_6_5_Window::Loop(float dt)
{

    time += dt;

    std::cout << "FPS: " << 1 / dt << std::endl;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_Program->Bind();
	m_VBA->Bind();

    float radius = 4.0f;

    glm::vec3 cameraPos = glm::vec3(radius * glm::sin(time), radius * glm::cos(time), radius * glm::sin(time));
    m_ViewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    m_Program->SetUniformMat4f("u_View", m_ViewMat);

	GLCall(glDrawArrays(GL_PATCHES, 0, 512));

    m_Grid->BindAndDraw(this->m_ProjMat, this->m_ViewMat);
    m_Grid->Unbind();

}

void core::Example_6_5_Window::OnKeyAction(int key, int scancode, int action, int mods)
{
	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
	}

}

void core::Example_6_5_Window::OnMouseMoveAction(double xpos, double ypos)
{
}

void core::Example_6_5_Window::OnMouseButtonAction(int button, int action, int mods)
{
}

void core::Example_6_5_Window::OnScrollAction(double xoffset, double yoffset)
{
}




//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////


void core::Example_6_5_Window::generatePatches(std::vector<GLfloat>& pts) {
    int idx = 0;

    // Build each patch
    // The rim
    buildPatchReflect(0, pts, idx, true, true);
    // The body
    buildPatchReflect(1, pts, idx, true, true);
    buildPatchReflect(2, pts, idx, true, true);
    // The lid
    buildPatchReflect(3, pts, idx, true, true);
    buildPatchReflect(4, pts, idx, true, true);
    // The bottom
    buildPatchReflect(5, pts, idx, true, true);
    // The handle
    buildPatchReflect(6, pts, idx, false, true);
    buildPatchReflect(7, pts, idx, false, true);
    // The spout
    buildPatchReflect(8, pts, idx, false, true);
    buildPatchReflect(9, pts, idx, false, true);
}

void core::Example_6_5_Window::buildPatchReflect(int patchNum,
    std::vector<GLfloat>& p, int& index, bool reflectX, bool reflectY)
{
    glm::vec3 patch[4][4];
    glm::vec3 patchRevV[4][4];
    getPatch(patchNum, patch, false);
    getPatch(patchNum, patchRevV, true);

    // Patch without modification
    buildPatch(patchRevV, p, index, glm::mat3(1.0f));

    // Patch reflected in x
    if (reflectX) {
        buildPatch(patch, p,
            index, glm::mat3(glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    // Patch reflected in y
    if (reflectY) {
        buildPatch(patch, p,
            index, glm::mat3(glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    // Patch reflected in x and y
    if (reflectX && reflectY) {
        buildPatch(patchRevV, p,
            index, glm::mat3(glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f)));
    }
}

void core::Example_6_5_Window::buildPatch(glm::vec3 patch[][4],
    std::vector<GLfloat>& pts, int& index, glm::mat3 reflect)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            glm::vec3 pt = reflect * patch[i][j];

            pts[index] = pt.x;
            pts[index + 1] = pt.y;
            pts[index + 2] = pt.z;

            index += 3;
        }
    }
}

void core::Example_6_5_Window::getPatch(int patchNum, glm::vec3 patch[][4], bool reverseV)
{
    for (int u = 0; u < 4; u++) {          // Loop in u direction
        for (int v = 0; v < 4; v++) {     // Loop in v direction
            if (reverseV) {
                patch[u][v] = glm::vec3(
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + (3 - v)]][0],
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + (3 - v)]][1],
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + (3 - v)]][2]
                );
            }
            else {
                patch[u][v] = glm::vec3(
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + v]][0],
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + v]][1],
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + v]][2]
                );
            }
        }
    }
}