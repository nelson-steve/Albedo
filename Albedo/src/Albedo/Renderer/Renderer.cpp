#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Albedo {

	void Renderer::Init(const entt::registry& reg)
	{
		auto group = reg.view<MeshComponent, ShaderComponent>();

		for (auto view : group)
		{
			if (group.get<MeshComponent>(view).m_Mesh->GetInitializationStatus())
			{
				group.get<MeshComponent>(view).m_Mesh->InitMesh(group.get<MeshComponent>(view).ID);
				group.get<MeshComponent>(view).m_Mesh->SetInitializationStatus(false);
			}
		}

		for (auto view : group)
		{
			auto& shader = group.get<ShaderComponent>(view);

			if (!shader.m_Shader->GetInitializationStatus()) continue;

			shader.m_Shader->Bind();
			//shader.m_Shader->SetUniformInt1("u_Default", 0);
			//shader.m_Shader->SetUniformInt1("u_AlbedoMap", 1);
			//shader.m_Shader->SetUniformInt1("u_AOMap", 2);
			//shader.m_Shader->SetUniformInt1("u_MetallicMap", 3);
			//shader.m_Shader->SetUniformInt1("u_NormalMap", 4);
			//shader.m_Shader->SetUniformInt1("u_RoughnessMap", 5);

			//glm::vec3 translations[100];
			//int index = 0;
			//float offset = 1.0f;
			//for (int y = -100; y < 100; y += 20)
			//{
			//	for (int x = -100; x < 100; x += 20)
			//	{
			//		glm::vec3 translation;
			//		translation.x = (float)(x + offset) / 10.0f;
			//		translation.y = (float)(y + offset) / 10.0f;
			//		translation.z = 0.0;
			//		translations[index++] = translation;
			//	}
			//}
			//
			//for (unsigned int i = 0; i < 100; i++)
			//{
			//	const std::string& n = ("offsets[" + std::to_string(i) + "]");
			//	shader.m_Shader->SetUniformFloat3(n, translations[i]);
			//}

			shader.m_Shader->SetUniformInt1("u_AbedoMap", 0);
			shader.m_Shader->SetUniformInt1("u_AOMap", 1);
			shader.m_Shader->SetUniformInt1("u_MetallicMap", 2);
			shader.m_Shader->SetUniformInt1("u_NormalMap", 3);
			shader.m_Shader->SetUniformInt1("u_RoughnessMap", 4);

			shader.m_Shader->SetInitializationStatus(false);
		}
	}

	GLenum Renderer::AlbedoDrawTypeToGLType(DrawType type)
	{
		switch (type)
		{
			case Albedo::DrawType::Albedo_POINTS:					return GL_POINTS;
			case Albedo::DrawType::Albedo_LINE_STRIP:				return GL_LINE_STRIP;
			case Albedo::DrawType::Albedo_LINE_LOOP:				return GL_LINE_LOOP;
			case Albedo::DrawType::Albedo_LINES:					return GL_LINES;
			case Albedo::DrawType::Albedo_LINE_STRIP_ADJACENCY:		return GL_LINE_STRIP_ADJACENCY;
			case Albedo::DrawType::Albedo_LINES_ADJACENCY:			return GL_LINES_ADJACENCY;
			case Albedo::DrawType::Albedo_TRIANGLE_STRIP:			return GL_TRIANGLE_STRIP;
			case Albedo::DrawType::Albedo_TRIANGLE_FAN:				return GL_TRIANGLE_FAN;
			case Albedo::DrawType::Albedo_TRIANGLES:				return GL_TRIANGLES;
			case Albedo::DrawType::Albedo_TRIANGLE_STRIP_ADJACENCY:	return GL_TRIANGLE_STRIP_ADJACENCY;
			case Albedo::DrawType::Albedo_TRIANGLES_ADJACENCY:		return GL_TRIANGLES_ADJACENCY;
			case Albedo::DrawType::Albedo_PATCHES:					return GL_PATCHES;
			default: 
				Albedo_Core_ERROR("Error: Unknown Rendering Type");
				return GL_TRIANGLES;
		}
	}

	void Renderer::Setup(const EditorCamera& camera, const ShaderComponent& shader,
		const TransformComponent& transform, const TextureComponent& texture, const MaterialComponent& material)
	{
		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
		//shader.m_Shader->SetUniformMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transform.GetTransform()))));
		shader.m_Shader->SetUniformFloat3("u_LightPosition", material.lightPos);
		shader.m_Shader->SetUniformFloat3("u_LightColor", glm::vec3(1.0, 1.0, 1.0));
		shader.m_Shader->SetUniformFloat("u_Exposure", material.m_Material->GetExposure());
		shader.m_Shader->SetUniformFloat("u_RoughnessScale", material.m_Material->GetRoughnessScale());
		shader.m_Shader->SetUniformFloat3("u_CameraPosition", camera.GetPosition());

		//shader.m_Shader->Bind();
		//shader.m_Shader->SetUniformMat4("projection", camera.GetProjection());
		//shader.m_Shader->SetUniformMat4("view", camera.GetViewMatrix());
		//shader.m_Shader->SetUniformFloat3("camPos", camera.GetPosition());
		//shader.m_Shader->SetUniformMat4("model", transform.GetTransform());
		//shader.m_Shader->SetUniformMat4("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transform.GetTransform()))));
		//shader.m_Shader->SetUniformFloat3("lightPositions", material.lightPos);
		//shader.m_Shader->SetUniformFloat3("lightColors", glm::vec3(1.0));

		//shader.m_Shader->SetUniformInt1("u_AlbedoMap", 0);

		if(texture.m_Textures.size() <= texture.totalTypes)
		{
			for (auto& it : texture.m_Textures)
			{
				if (it.second)
					it.second->Bind(it.first);
			}
		}
	}

	void Renderer::Setup(const SceneCamera& camera, const ShaderComponent& shader, const TransformComponent& transform,
		const TextureComponent& texture, const MaterialComponent& material)
	{
		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera.GetProjectionView());
		shader.m_Shader->SetUniformInt1("u_AlbedoMap", 0);
		shader.m_Shader->SetUniformInt1("u_AOMap", 1);
		shader.m_Shader->SetUniformInt1("u_MetallicMap", 2);
		shader.m_Shader->SetUniformInt1("u_NormalMap", 3);
		shader.m_Shader->SetUniformInt1("u_RoughnessMap", 4);

		if (texture.m_Textures.size() <= texture.totalTypes)
		{
			for (auto& it : texture.m_Textures)
			{
				if (it.second)
					it.second->Bind(it.first);
			}
		}
	}

	void Renderer::Setup(const EditorCamera& camera, const Ref<Shader> shader, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetUniformMat4("u_Transform", transform);
		shader->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
		shader->SetUniformInt1("u_Default", 0);
	}

	void Renderer::RenderOverlay(const Ref<Mesh> mesh)
	{
		mesh->GetMeshBufferData().m_VertexArray->Bind();
		glDrawArrays(AlbedoDrawTypeToGLType(mesh->GetRendererConfig().Type), 0, mesh->GetVertices().size());
		mesh->GetMeshBufferData().m_VertexArray->UnBind();
	}

	void Renderer::Render(const MeshComponent& mesh, RendererConfig config)
	{
		if(config.PolygonMode)	
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mesh.m_Mesh->GetMeshBufferData().m_VertexArray->Bind();
		if(mesh.m_Mesh->IsSingularData())
			glDrawArrays(AlbedoDrawTypeToGLType(config.Type), 0, sizeof(mesh.m_Mesh->GetSingularMeshData()));
		else
		{
			glDrawElements(AlbedoDrawTypeToGLType(config.Type), mesh.m_Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0);
			//glDrawArrays(AlbedoDrawTypeToGLType(config.Type), 0, mesh.m_Mesh->GetVertices().size());
			//glDrawElementsInstanced();
			//glDrawArraysInstanced(AlbedoDrawTypeToGLType(config.Type), 0, mesh.m_Mesh->GetVertices().size(), 100);
			//glDrawArraysInstanced(AlbedoDrawTypeToGLType(config.Type), 0, mesh.m_Mesh->GetVertices().size(), 100);
		}
		mesh.m_Mesh->GetMeshBufferData().m_VertexArray->UnBind();
		if(config.PolygonMode) 
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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