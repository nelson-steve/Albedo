#include "AlbedoPreCompiledHeader.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Albedo {

	void OpenGLRendererAPI::Init()
	{
		Albedo_PROFILE_FUNCTION();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		Albedo_PROFILE_FUNCTION();
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		Albedo_PROFILE_FUNCTION();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		Albedo_PROFILE_FUNCTION();
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		Albedo_PROFILE_FUNCTION();
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}