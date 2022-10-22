#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"

namespace Albedo {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData();
	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ProjectionViewMatrix = camera.GetProjectionViewMatrix();
	}
	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ProjectionView", s_SceneData->ProjectionViewMatrix);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}