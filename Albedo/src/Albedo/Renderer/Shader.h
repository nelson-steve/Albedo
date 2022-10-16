#pragma once

namespace Albedo {

	class Shader
	{
	public:
		Shader(const char* VertexSrc, const char* fragmentSrc);

		void Bind();
		void Unbind();

		void UploadMat4f();
	private:
		unsigned int vertexShader, fragmentShader, shaderProgram;
	};

}