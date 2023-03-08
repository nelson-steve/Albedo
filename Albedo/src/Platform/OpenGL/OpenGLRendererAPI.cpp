#include "AlbedoPreCompiledHeader.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Albedo {

	void OpenGLRendererAPI::Init()
	{
		Albedo_PROFILE_FUNCTION();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		//glFrontFace(GL_CW);

		glEnable(GL_DEPTH_TEST);

		Albedo_Core_INFO("	OpenGL Info:");
		Albedo_Core_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		Albedo_Core_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		Albedo_Core_INFO("  Version: {0}", glGetString(GL_VERSION));
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

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		Albedo_PROFILE_FUNCTION();
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		Albedo_PROFILE_FUNCTION();
#if 0
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
#endif

#if 1
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount	();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
#endif
	}

}