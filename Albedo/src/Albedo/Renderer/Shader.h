#pragma once

namespace Albedo {

	class Shader
	{
	public:
		Shader(const char* VertexSrc, const char* fragmentSrc);
		~Shader();

		void Bind();
		void Unbind();

		void UploadUniformdMat4f();
	private:
		unsigned int m_ShaderID;
	};

}