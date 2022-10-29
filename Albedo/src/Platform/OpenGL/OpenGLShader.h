#pragma once

#include "Albedo/Renderer/Shader.h"
#include <glm/glm.hpp>

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Albedo {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader::OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& name, const char* vertexSrc, const char* fragmentSrc);
		~OpenGLShader();

		virtual void Bind() override;
		virtual void Unbind() override;

		const std::string& GetName() const { return m_Name; }

		void UploadUniformInt1(const std::string& name, int value);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		std::string readFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		unsigned int m_ShaderID;
		std::string m_Name;
	};

}