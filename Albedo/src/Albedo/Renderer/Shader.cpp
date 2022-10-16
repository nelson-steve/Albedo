#include "AlbedoPreCompiledHeader.h"

#include "Albedo/Renderer/Shader.h"

#include <glad/glad.h>

namespace Albedo {

	Shader::Shader(const char* VertexSrc, const char* fragmentSrc)
	{
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShader, 1, &VertexSrc, NULL);
		glCompileShader(vertexShader);

		int  success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}


		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
		glCompileShader(fragmentShader);

		shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

		glUseProgram(shaderProgram);
	}

	void Shader::Bind()
	{

	}

	void Shader::Unbind()
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void Shader::UploadMat4f()
	{

	}

}