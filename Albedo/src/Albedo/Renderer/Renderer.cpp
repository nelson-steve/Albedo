#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Albedo {

	void Renderer::Init(const entt::registry& reg)
	{
		auto group = reg.view<MeshComponent>();

		for (auto view : group)
		{
			if (group.get<MeshComponent>(view).m_Mesh->GetInitializationStatus())
			{
				group.get<MeshComponent>(view).m_Mesh->InitMesh();
				group.get<MeshComponent>(view).m_Mesh->SetInitializationStatus(false);
			}
		}	
	}

	void Renderer::Setup(const EditorCamera& camera, const ShaderComponent& shader, const TransformComponent& transform, const TextureComponent& texture)
	{
		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
		shader.m_Shader->SetUniformInt1("u_AlbedoMap", 0);


		texture.m_Textures[0]->Bind();

		//scnObj->GetShader()->SetUniformInt1("u_DiffuseMap", 0);
		//scnObj->GetShader()->SetUniformInt1("u_AOMap", 1);
		//scnObj->GetShader()->SetUniformInt1("u_MetallicMap", 2);
		//scnObj->GetShader()->SetUniformInt1("u_NormalMap", 3);
		//scnObj->GetShader()->SetUniformInt1("u_RoughnessMap", 4);
		
		//scnObj->GetShader()->SetUniformFloat("u_RoughnessScale", 0.0);
		//scnObj->GetShader()->SetUniformFloat3("u_LightPosition", lightPos);
		//scnObj->GetShader()->SetUniformFloat3("u_LightColor", glm::vec3(1.0, 1.0, 1.0));
		//scnObj->GetShader()->SetUniformFloat("u_Exposure", 3.5);
	}

	void Renderer::Setup(const SceneCamera& camera, const ShaderComponent& shader, const TransformComponent& transform, const TextureComponent& texture)
	{
		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera.GetProjectionView());
		shader.m_Shader->SetUniformInt1("u_AlbedoMap", 0);

		texture.m_Textures[0]->Bind();
	}

	void Renderer::Render(const MeshComponent& mesh)
	{
			mesh.m_Mesh->GetMeshBufferData().m_VertexArray->Bind();
		if(mesh.m_Mesh->IsSingularData())
			glDrawArrays(GL_TRIANGLES, 0, sizeof(mesh.m_Mesh->GetSingularMeshData()));
		else
			glDrawArrays(GL_TRIANGLES, 0, mesh.m_Mesh->GetVertices().size());
	}

	void Renderer::Shutdown()
	{

	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		Albedo_PROFILE_FUNCTION();
		RenderCommand::SetViewPort(720, 1280, width, height);
	}

}