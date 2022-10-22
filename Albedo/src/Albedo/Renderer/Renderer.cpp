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

	void Renderer::Submit(const std::shared_ptr<OpenGLShader>& shader, 
		const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, glm::vec4& color)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ProjectionView", s_SceneData->ProjectionViewMatrix);
		shader->UploadUniformMat4("u_Transform", transform);
		shader->UploadUniformFloat4("u_Color", color);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}