#pragma once

#include "Albedo/Renderer/Shader.h"

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Albedo {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const char* vertexSrc, const char* fragmentSrc);
		OpenGLShader(const std::string& filePath);
		~OpenGLShader();

		virtual void Bind()	  override;
		virtual void Unbind() override;

		virtual const std::string& GetName() const override { return m_Name; }
		virtual const std::string& GetPath() const override { return m_Path; }

		virtual void SetUniformInt1(const std::string& name, int value) override;
		virtual void SetUniformIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetUniformFloat (const std::string& name, float value)			   override;
		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetUniformMat3(const std::string& name, const glm::mat3& matrix) override;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& matrix) override;
		virtual void SetPath(const std::string& path) override { m_Path = path; }

		//----------------------------------------------------------------------//

		void UploadUniformInt1(const std::string& name, int value);

		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void UploadUniformFloat (const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

		inline unsigned int GetShaderID() const { return m_ShaderID; }
	private:
		std::string readFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		unsigned int m_ShaderID;
		std::string m_Name;
		std::string m_Path;
	};

}