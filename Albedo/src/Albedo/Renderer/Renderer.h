#pragma once

#include "RenderCommand.h"
#include "Shader.h"
#include "OrthographicCamera.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Albedo {
	class Renderer 
	{
	public:
		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<OpenGLShader>& shader, const Ref<OpenGLTexture2D>& texture,
			const Ref<VertexArray>& vertexArray, const glm::mat4& transform, glm::vec4& color);
		static void Submit(const Ref<OpenGLShader>& shader,
			const Ref<VertexArray>& vertexArray, const glm::mat4& transform, glm::vec4& color);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ProjectionViewMatrix;
		};

		static SceneData* s_SceneData;
	};
}