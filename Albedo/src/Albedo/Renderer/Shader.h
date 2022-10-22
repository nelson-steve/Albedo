#pragma once

#include <glm/glm.hpp>

namespace Albedo {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		static Shader* Create(const char* vertexSrc, const char* fragmentSrc);
	};

}