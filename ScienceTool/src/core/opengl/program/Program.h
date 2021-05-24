#pragma once

#include "../core/bindable/Bindable.h"
#include "array-list/ArrayList.h"

#include <string>

#include <../../Math/src/glm/glm.hpp>
#include <../../Math/src/glm/gtc/matrix_transform.hpp>

namespace opengl
{

	struct ShaderProgramSource
	{
		std::string VertexSource;
		std::string FragmentSource;
	};

	class Program : Bindable
	{
	public:
		Program(const std::string& filepath);
		Program(const std::string& baseFilePath, const std::string& vs, const std::string& fs);
		Program(const std::string& baseFilePath, const std::string& vs, const std::string& fs, void* param);
		Program(const std::string& baseFilePath, const std::string& vs, const std::string& fs, const std::string& gs);
		Program(const std::string& baseFilePath, const std::string& vs, const std::string& fs, const std::string& tcs, const std::string& tes);
		Program(const std::string& baseFilePath, const std::string& vs, const std::string& fs, const std::string& gs, const std::string& tcs, const std::string& tes);
		Program(const std::string& baseFilePath, const std::string& cs);
		~Program();

	public:
		void Bind() const;
		void Unbind() const;

	public:
		int GetUniformLocation(const std::string& name);
	public:
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform4f(const std::string& name, glm::vec4 vec);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform3f(const std::string& name, glm::vec3 vec);
		void SetUniform1f(const std::string& name, float v);
		void SetUniform1i(const std::string& name, int b);
		void SetUniform1ui(const std::string& name, unsigned int b);

		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
	private:
		void Create();
		void AttachShader(unsigned int shaderId);
	public:
		void LinkAndValidate();
	private:
		ShaderProgramSource ParseShaders(const std::string& filepath);
		std::string ParseShader(const std::string& filepath);
		unsigned int CompileShader(unsigned int type, const std::string& source);

	private:
		std::string m_FilePath;
		collection::ArrayList<int> m_AttachedShaderIds;
	};


}

