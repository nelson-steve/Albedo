#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
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

			//shader.m_Shader->SetUniformInt1("u_AOMap", 1);
			//shader.m_Shader->SetUniformInt1("u_MetallicMap", 2);
			//shader.m_Shader->SetUniformInt1("u_NormalMap", 3);
			//shader.m_Shader->SetUniformInt1("u_RoughnessMap", 4);
			shader.m_Shader->SetUniformInt1("diffuseTexture", 0);
			shader.m_Shader->SetUniformInt1("shadowMap", 1);

			shader.m_Shader->SetInitializationStatus(false);
		}
	}

	void Renderer::PreRenderPass(Ref<Shader> depthShader, Ref<ShadowMap> fbo, 
		const entt::registry& reg, const glm::vec3& l, Ref<Texture2D> tex)
	{
		auto view = reg.view<TransformComponent, MeshComponent>();

		fbo->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 70.0f;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(l, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		depthShader->Bind();
		depthShader->SetUniformMat4("u_LightSpaceMatrix", lightSpaceMatrix);
		for (auto& entity : view)
		{
			//if (view.<LightComponent>(entity))
			//	continue;
			auto& mesh = view.get<MeshComponent>(entity);
			auto& transform = view.get<TransformComponent>(entity);
			depthShader->SetUniformMat4("u_Transform", transform.GetTransform());
			tex->Bind();
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, woodTexture);
			mesh.m_Mesh->GetMeshBufferData().m_VertexArray->Bind();
			glDrawArrays(AlbedoDrawTypeToGLType(mesh.m_Mesh->GetRendererConfig().Type), 0, mesh.m_Mesh->GetVertices().size());
			mesh.m_Mesh->GetMeshBufferData().m_VertexArray->UnBind();
		}

		//Render(simpleDepthShader);
		
		depthShader->Unbind();
		fbo->Unbind();
		//glBindFramebuffer(GL_FRAMEBUFFER, 1);

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
		const TransformComponent& transform, const TextureComponent& texture, const MaterialComponent& material, 
		const std::vector<LightComponent>& lights)
	{
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 70.0f;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::vec3 l;
		if (lights.empty())
			l = glm::vec3(1.0f, 3.0f, 0.0f);
		else
			l = lights[0].position;
		lightView = glm::lookAt(l, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_LightSpaceMatrix", lightSpaceMatrix);
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
		//shader.m_Shader->SetUniformMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transform.GetTransform()))));
		//shader.m_Shader->SetUniformInt1("u_NoOfLights", lights.size());	
		int i = 0;
		for (const LightComponent& l : lights)
		{
			//std::string& s = "u_LightPosition[" + std::to_string(i) + "]";
			shader.m_Shader->SetUniformFloat3("u_LightPos", l.position);

			//s = "u_LightColor[" + std::to_string(i) + "]";
			//shader.m_Shader->SetUniformFloat3(s, l.ambient);
			shader.m_Shader->SetUniformFloat3("u_MaterialColor", l.ambient);

			i++;
		}
		
		//shader.m_Shader->SetUniformFloat3("u_LightColor", glm::vec3(1.0, 1.0, 1.0));
		//shader.m_Shader->SetUniformFloat("u_Exposure", material.m_Material->GetExposure());
		//shader.m_Shader->SetUniformFloat("u_RoughnessScale", material.m_Material->GetRoughnessScale());
		shader.m_Shader->SetUniformFloat3("u_CameraPos", camera.GetPosition());

		//shader.m_Shader->Bind();
		//shader.m_Shader->SetUniformMat4("projection", camera.GetProjection());
		//shader.m_Shader->SetUniformMat4("view", camera.GetViewMatrix());
		//shader.m_Shader->SetUniformFloat3("camPos", camera.GetPosition());
		//shader.m_Shader->SetUniformMat4("model", transform.GetTransform());
		//shader.m_Shader->SetUniformMat4("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transform.GetTransform()))));
		//shader.m_Shader->SetUniformFloat3("lightPositions", material.lightPos);
		//shader.m_Shader->SetUniformFloat3("lightColors", glm::vec3(1.0));

		//shader.m_Shader->SetUniformInt1("u_AlbedoMap", 0);

		//if(texture.m_Textures.size() <= texture.totalTypes)
		//{
		//	for (auto& it : texture.m_Textures)
		//	{
		//		if (it.second)
		//			it.second->Bind(it.first);
		//	}
		//}


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
			//glDrawElements(AlbedoDrawTypeToGLType(config.Type), mesh.m_Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0);
			//glDrawArrays(AlbedoDrawTypeToGLType(config.Type), 0, mesh.m_Mesh->GetVertices().size());
			//glDrawElementsInstanced(AlbedoDrawTypeToGLType(config.Type), mesh.m_Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0, 1);
			glDrawArrays(AlbedoDrawTypeToGLType(config.Type), 0, mesh.m_Mesh->GetVertices().size());
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