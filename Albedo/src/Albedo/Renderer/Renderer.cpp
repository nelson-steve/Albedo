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
			shader.m_Shader->Bind();
			shader.m_Shader->SetUniformInt1("u_Default", 0);
			shader.m_Shader->SetUniformInt1("u_AlbedoMap", 1);
			shader.m_Shader->SetUniformInt1("u_AOMap", 2);
			shader.m_Shader->SetUniformInt1("u_MetallicMap", 3);
			shader.m_Shader->SetUniformInt1("u_NormalMap", 4);
			shader.m_Shader->SetUniformInt1("u_RoughnessMap", 5);
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

	void Renderer::Setup(const EditorCamera& camera, const ShaderComponent& shader, const TransformComponent& transform, const TextureComponent& texture)
	{
		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
		shader.m_Shader->SetUniformInt1("u_DefaultTexture", (int)texture.defaultTexture);
		//shader.m_Shader->SetUniformInt1("u_AlbedoMap", 0);


		if (texture.m_Textures.empty() || texture.defaultTexture && texture.m_DefaultTexture)
			texture.m_DefaultTexture->Bind();
		else if(texture.m_Textures.size() <= texture.totalTypes)
		{
			int i = 0;
			for (auto& it : texture.m_Textures) {
				if (it.second)
					it.second->Bind(it.first);
			}
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

	void Renderer::Setup(const SceneCamera& camera, const ShaderComponent& shader, const TransformComponent& transform, const TextureComponent& texture)
	{
		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera.GetProjectionView());
		shader.m_Shader->SetUniformInt1("u_Default", 0);
		shader.m_Shader->SetUniformInt1("u_AlbedoMap", 1);
		shader.m_Shader->SetUniformInt1("u_AOMap", 2);
		shader.m_Shader->SetUniformInt1("u_MetallicMap", 3);
		shader.m_Shader->SetUniformInt1("u_NormalMap", 4);
		shader.m_Shader->SetUniformInt1("u_RoughnessMap", 5);

		if (texture.m_Textures.empty() || texture.defaultTexture && texture.m_DefaultTexture)
			texture.m_DefaultTexture->Bind();
		else if (texture.m_Textures.size() <= 5)
		{
			int i = 0;
			for (auto& it : texture.m_Textures) {
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
			glDrawArrays(AlbedoDrawTypeToGLType(config.Type), 0, mesh.m_Mesh->GetVertices().size());
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