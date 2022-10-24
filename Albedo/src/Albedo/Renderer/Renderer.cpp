#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Albedo {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData();
	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ProjectionViewMatrix = camera.GetProjectionViewMatrix();
	}
	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const Ref<OpenGLShader>& shader, const Ref<OpenGLTexture2D>& texture,
		const Ref<VertexArray>& vertexArray, const glm::mat4& transform, glm::vec4& color)
	{
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
		shader->Bind();
		shader->UploadUniformMat4("u_ProjectionView", s_SceneData->ProjectionViewMatrix);
		shader->UploadUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}