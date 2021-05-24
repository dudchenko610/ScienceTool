#include "Program.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "../core/error/ErrorChecker.h"

opengl::ShaderProgramSource opengl::Program::ParseShaders(const std::string& filepath)
{
	std::fstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };

}

std::string opengl::Program::ParseShader(const std::string& filepath)
{
	std::fstream stream(filepath);

	std::string line;
	std::stringstream ss;

	while (getline(stream, line))
	{
		ss << line << '\n';
	}

	return ss.str();
}

/*

The first step is to create the shader object using the glCreateShader function. The argument
is the type of shader, and can be one of the following: GL_VERTEX_SHADER, GL_FRAGMENT_
SHADER, GL_GEOMETRY_SHADER, GL_TESS_EVALUATION_SHADER, GL_TESS_CONTROL_
SHADER, or (as of version 4.3) GL_COMPUTE_SHADER. In this case, since we are compiling a
vertex shader, we use GL_VERTEX_SHADER. This function returns the value used for referencing
the vertex shader object, sometimes called the object "handle". We store that value in the
variable vertShader. If an error occurs while creating the shader object, this function will
return 0, so we check for that and if it occurs, we print an appropriate message and terminate.
Following the creation of the shader object, we load the source code into the shader object
using the function glShaderSource. This function is designed to accept an array of strings (as
opposed to just a single one) in order to support the option of compiling multiple sources (files,
strings) at once. So before we call glShaderSource, we place a pointer to our source code into
an array named sourceArray. The first argument to glShaderSource is the handle to the
shader object. The second is the number of source code strings that are contained in the array.
The third argument is a pointer to an array of source code strings. The final argument is an array
of GLint values that contains the length of each source code string in the previous argument.
In the previous code, we pass a value of NULL, which indicates that each source code string
is terminated by a null character. If our source code strings were not null terminated then this
argument must be a valid array. Note that once this function returns, the source code has been
copied into OpenGL internal memory, so the memory used to store the source code can be freed.

*/


unsigned int opengl::Program::CompileShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();

	GLCall(glShaderSource(id, 1, &src, nullptr)); // nullptr - means that src should be null terminated
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

		char* message = (char*)alloca(length * sizeof(char)); // stack allocated

		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;

		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}


void opengl::Program::Create()
{
	GLCall(unsigned int program = glCreateProgram());

	this->glId = program;

	if (program == 0)
	{
		fprintf(stderr, "Error creating program object.\n");
		exit(1);
	}
}

void opengl::Program::AttachShader(unsigned int shaderId)
{
	GLCall(glAttachShader(this->glId, shaderId));
	this->m_AttachedShaderIds.Add(shaderId);

}

void opengl::Program::LinkAndValidate()
{

	GLCall(glLinkProgram(this->glId));
	GLCall(glValidateProgram(this->glId));

	int status;
	glGetProgramiv(this->glId, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {

		fprintf(stderr, "Failed to link shader program!\n");

		int logLen;
		glGetProgramiv(this->glId, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			char* log = new char[logLen]; // heap allocated

			int written;
			glGetProgramInfoLog(this->glId, logLen, &written, log);

			fprintf(stderr, "Program log: \n%s", log);

			delete[] log;
		}
	}

	this->m_AttachedShaderIds.ForEach(
		[](int shaderId)
		{
			GLCall(glDeleteShader(shaderId));
		}
	);

	//this->m_AttachedShaderIds.Clear();

}

opengl::Program::Program(const std::string& filepath)
	: m_FilePath(filepath)
{
	ShaderProgramSource source = ParseShaders(m_FilePath);

	this->Create();

	unsigned int vs = CompileShader(GL_VERTEX_SHADER, source.VertexSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, source.FragmentSource);

	this->AttachShader(vs);
	this->AttachShader(fs);

	this->LinkAndValidate();
}

opengl::Program::Program(const std::string& baseFilePath, const std::string& vs, const std::string& fs)
	: m_FilePath(baseFilePath)
{
	std::string vsPath = std::string(baseFilePath) + "/" + vs;
	std::string fsPath = std::string(baseFilePath) + "/" + fs;

	this->Create();

	unsigned int vsId = CompileShader(GL_VERTEX_SHADER, this->ParseShader(vsPath));
	unsigned int fsId = CompileShader(GL_FRAGMENT_SHADER, this->ParseShader(fsPath));

	this->AttachShader(vsId);
	this->AttachShader(fsId);

	this->LinkAndValidate();
}

opengl::Program::Program(const std::string& baseFilePath, const std::string& cs)
	: m_FilePath(baseFilePath)
{
	std::string csPath = std::string(baseFilePath) + "/" + cs;

	this->Create();
	unsigned int csId = CompileShader(GL_COMPUTE_SHADER, this->ParseShader(csPath));

	this->AttachShader(csId);
	this->LinkAndValidate();
}

opengl::Program::Program(const std::string& baseFilePath, const std::string& vs, const std::string& fs, void* param)
	: m_FilePath(baseFilePath)
{
	std::string vsPath = std::string(baseFilePath) + "/" + vs;
	std::string fsPath = std::string(baseFilePath) + "/" + fs;

	this->Create();

	unsigned int vsId = CompileShader(GL_VERTEX_SHADER, this->ParseShader(vsPath));
	unsigned int fsId = CompileShader(GL_FRAGMENT_SHADER, this->ParseShader(fsPath));

	this->AttachShader(vsId);
	this->AttachShader(fsId);

	//this->LinkAndValidate();
}

opengl::Program::Program(const std::string& baseFilePath, const std::string& vs, const std::string& fs, const std::string& gs)
	: m_FilePath(baseFilePath)
{
	std::string vsPath = std::string(baseFilePath) + "/" + vs;
	std::string fsPath = std::string(baseFilePath) + "/" + fs;
	std::string gsPath = std::string(baseFilePath) + "/" + gs;

	this->Create();

	unsigned int vsId = CompileShader(GL_VERTEX_SHADER, this->ParseShader(vsPath));
	unsigned int fsId = CompileShader(GL_FRAGMENT_SHADER, this->ParseShader(fsPath));
	unsigned int gsId = CompileShader(GL_GEOMETRY_SHADER, this->ParseShader(gsPath));

	this->AttachShader(vsId);
	this->AttachShader(fsId);
	this->AttachShader(gsId);

	this->LinkAndValidate();
}

opengl::Program::Program(const std::string& baseFilePath, const std::string& vs, const std::string& fs, const std::string& tcs, const std::string& tes)
	: m_FilePath(baseFilePath)
{
	std::string vsPath = std::string(baseFilePath) + "/" + vs;
	std::string fsPath = std::string(baseFilePath) + "/" + fs;
	std::string tcsPath = std::string(baseFilePath) + "/" + tcs;
	std::string tesPath = std::string(baseFilePath) + "/" + tes;

	this->Create();

	unsigned int vsId = CompileShader(GL_VERTEX_SHADER, this->ParseShader(vsPath));
	unsigned int fsId = CompileShader(GL_FRAGMENT_SHADER, this->ParseShader(fsPath));
	unsigned int tcsId = CompileShader(GL_TESS_CONTROL_SHADER, this->ParseShader(tcsPath));
	unsigned int tesId = CompileShader(GL_TESS_EVALUATION_SHADER, this->ParseShader(tesPath));

	this->AttachShader(vsId);
	this->AttachShader(fsId);
	this->AttachShader(tcsId);
	this->AttachShader(tesId);

	this->LinkAndValidate();
}

opengl::Program::Program(const std::string& baseFilePath, const std::string& vs, const std::string& fs, const std::string& gs, const std::string& tcs, const std::string& tes)
	: m_FilePath(baseFilePath)
{

	

	std::string vsPath = std::string(baseFilePath) + "/" + vs;
	std::string fsPath = std::string(baseFilePath) + "/" + fs;
	std::string gsPath = std::string(baseFilePath) + "/" + gs;

	std::string tcsPath = std::string(baseFilePath) + "/" + tcs;
	std::string tesPath = std::string(baseFilePath) + "/" + tes;

	this->Create();

	unsigned int vsId = CompileShader(GL_VERTEX_SHADER, this->ParseShader(vsPath));
	unsigned int fsId = CompileShader(GL_FRAGMENT_SHADER, this->ParseShader(fsPath));
	unsigned int gsId = CompileShader(GL_GEOMETRY_SHADER, this->ParseShader(gsPath));

	unsigned int tcsId = CompileShader(GL_TESS_CONTROL_SHADER, this->ParseShader(tcsPath));
	unsigned int tesId = CompileShader(GL_TESS_EVALUATION_SHADER, this->ParseShader(tesPath));

	this->AttachShader(vsId);
	this->AttachShader(fsId);
	this->AttachShader(gsId);
	this->AttachShader(tcsId);
	this->AttachShader(tesId);

	this->LinkAndValidate();
}



opengl::Program::~Program()
{
	GLCall(glDeleteProgram(glId));
}


void opengl::Program::Bind() const
{
	GLCall(glUseProgram(glId));
}

void opengl::Program::Unbind() const
{
	GLCall(glUseProgram(0));
}

int opengl::Program::GetUniformLocation(const std::string& name)
{

	/*if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}*/

	GLCall(int location = glGetUniformLocation(glId, name.c_str()));

	if (location == -1)
	{
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	}

	//m_UniformLocationCache[name] = location;
	return location;
}


void opengl::Program::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void opengl::Program::SetUniform4f(const std::string& name, glm::vec4 vec)
{
	GLCall(glUniform4f(GetUniformLocation(name), vec.x, vec.y, vec.z, vec.w));
}

void opengl::Program::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void opengl::Program::SetUniform3f(const std::string& name, glm::vec3 vec)
{
	GLCall(glUniform3f(GetUniformLocation(name), vec.x, vec.y, vec.z));
}

void opengl::Program::SetUniform1f(const std::string& name, float v)
{
	GLCall(glUniform1f(GetUniformLocation(name), v));
}

void opengl::Program::SetUniform1i(const std::string& name, int i)
{
	GLCall(glUniform1i(GetUniformLocation(name), i));
}

void opengl::Program::SetUniform1ui(const std::string& name, unsigned int i)
{
	GLCall(glUniform1ui(GetUniformLocation(name), i));
}

void opengl::Program::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	// GL_FALSE - we do not need to transpose,bacause glm stores matrix as column-major, like opengl does
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}