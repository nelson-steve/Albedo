#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Albedo {

	void Renderer::Init(const std::vector<Ref<SceneObject>> scnObjs)
	{
		//RenderCommand::Init();

		for (auto obj : scnObjs)
		{
			obj->GetMesh()->InitMesh();
		}
	}

	void Renderer::Init(const entt::registry& reg)
	{
		auto group = reg.view<MeshComponent>();

		for (auto view : group)
		{
			group.get<MeshComponent>(view).m_Mesh->InitMesh();
		}
		
	}

	void Renderer::Setup(const EditorCamera& camera, const ShaderComponent& shader, const TransformComponent& transform)
	{
		shader.m_Shader->Bind();
		glm::mat4 transform_(1.0);
		shader.m_Shader->SetUniformMat4("u_Transform", transform_);
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());

	}

	void Renderer::Setup(const EditorCamera& camera, const Ref<SceneObject> scnObj)
	{
		scnObj->GetShader()->Bind();

		glm::mat4 transform(1.0);
		transform = glm::translate(transform, scnObj->GetPosition());
		transform = glm::scale(transform, glm::vec3(1.0f));
		scnObj->GetShader()->SetUniformMat4("u_Transform", transform);
		scnObj->GetShader()->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
		//scnObj->GetShader()->SetUniformFloat3("u_CameraPosition", camera.GetPosition());

		scnObj->GetShader()->SetUniformInt1("u_AlbedoMap", 0);

		for (int i = 0; i<scnObj->GetTextures().size(); i++)
		{
			scnObj->GetTextures()[i]->Bind();
		}
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

	void Renderer::Render(const Ref<SceneObject> scnObj)
	{
		scnObj->GetMesh()->GetMeshBufferData().m_VertexArray->Bind();
		auto c = scnObj->GetMesh()->GetVertices().size();
		glDrawArrays(GL_TRIANGLES, 0, scnObj->GetMesh()->GetVertices().size());
	}

	void Renderer::Render(const MeshComponent& mesh)
	{
		mesh.m_Mesh->GetMeshBufferData().m_VertexArray->Bind();
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