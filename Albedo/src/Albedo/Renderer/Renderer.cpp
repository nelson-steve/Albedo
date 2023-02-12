#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Albedo/Renderer/Renderer2D.h"

namespace Albedo {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;
	
	void Renderer::Init()
	{
		Albedo_PROFILE_FUNCTION();
		RenderCommand::Init();
	#ifdef BATCH
		Renderer2D::Init();
	#endif

	#ifndef BATCH
		Renderer2D::Init();
	#endif // !BATCH

	}

	void Renderer::Shutdown()
	{
		Albedo_PROFILE_FUNCTION();
		#ifdef BATCH
		Renderer2D::Shutdown();
		#endif

		#ifndef BATCH
		Renderer2D::Shutdown();
		#endif
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		Albedo_PROFILE_FUNCTION();
		RenderCommand::SetViewPort(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		Albedo_PROFILE_FUNCTION();
		s_SceneData->ProjectionViewMatrix = camera.GetProjectionViewMatrix();
	}
	void Renderer::EndScene()
	{
		Albedo_PROFILE_FUNCTION();
	}

	void Renderer::Submit(const Ref<OpenGLShader>& shader, const Ref<OpenGLTexture2D>& texture,
		const Ref<VertexArray>& vertexArray, const glm::mat4& transform, glm::vec4& color)
	{
		Albedo_PROFILE_FUNCTION();
		//texture->Bind();
		
		shader->Bind();
		shader->UploadUniformInt1("u_Texture", 0);
		shader->UploadUniformMat4("u_ProjectionView", s_SceneData->ProjectionViewMatrix);
		shader->UploadUniformMat4("u_Transform", transform);
		//shader->UploadUniformFloat4("u_Color", color);


		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::Submit(const Ref<OpenGLShader>& shader,
		const Ref<VertexArray>& vertexArray, const glm::mat4& transform, glm::vec4& color)
	{
		Albedo_PROFILE_FUNCTION();
		shader->Bind();
		shader->UploadUniformMat4("u_ProjectionView", s_SceneData->ProjectionViewMatrix);
		shader->UploadUniformMat4("u_Transform", transform);
		shader->UploadUniformFloat4("u_Color", color);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}