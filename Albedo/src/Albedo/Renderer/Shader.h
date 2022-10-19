#pragma once

#include <glm/glm.hpp>

namespace Albedo {

	class Shader
	{
	public:
		Shader(const char* VertexSrc, const char* fragmentSrc);
		~Shader();

		void Bind();
		void Unbind();

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		unsigned int m_ShaderID;
	};

}