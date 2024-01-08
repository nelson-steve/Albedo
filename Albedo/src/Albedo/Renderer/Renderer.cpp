#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Albedo {

	void Renderer::Init(const entt::registry& reg)
	{
	}

	void Renderer::PreRenderPass(Ref<Shader> depthShader, Ref<ShadowMap> fbo,
		const entt::registry& reg, const glm::vec3& dir, Ref<Texture2D> tex)
	{
		auto view = reg.view<TransformComponent, ModelComponent>();

		fbo->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 70.0f;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(-dir, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		depthShader->Bind();
		depthShader->SetUniformMat4("u_LightSpaceMatrix", lightSpaceMatrix);
		for (auto& entity : view)
		{
			//if (view.<LightComponent>(entity))
			//	continue;
			auto& mesh = view.get<ModelComponent>(entity);
			auto& transform = view.get<TransformComponent>(entity);
			depthShader->SetUniformMat4("u_Transform", transform.GetTransform());
			//tex->Bind();
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, woodTexture);

			mesh.m_Model->Draw(depthShader);
			//mesh.m_Mesh->GetMeshBufferData().m_VertexArray->Bind();
			//glDrawArrays(AlbedoDrawTypeToGLType(mesh.m_Mesh->GetRendererConfig().Type), 0, mesh.m_Mesh->GetVertices().size());
			//mesh.m_Mesh->GetMeshBufferData().m_VertexArray->UnBind();
		}

		//Render(simpleDepthShader);
		
		depthShader->Unbind();
		fbo->Unbind();
		//glBindFramebuffer(GL_FRAMEBUFFER, 1);

	}

	void Renderer::SetupPBR(Camera* camera, const ShaderComponent& shader, const TransformComponent& transform,
		const TextureComponent& texture, const MaterialComponent& material, const std::vector<LightComponent>& lights)
	{
		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera->GetProjectionView());
		//shader.m_Shader->SetUniformFloat3("u_CameraPosition", camera.GetPosition());

		int i = 0;
		for (const LightComponent& l : lights)
		{
			std::string& s = "u_LightPosition[" + std::to_string(i) + "]";
			shader.m_Shader->SetUniformFloat3(s, l.position);

			s = "u_LightColor[" + std::to_string(i) + "]";
			shader.m_Shader->SetUniformFloat3(s, l.ambient);
			i++;
		}
		//shader.m_Shader->SetUniformFloat("u_Exposure", material.m_Material->GetExposure());
		shader.m_Shader->SetUniformFloat("u_Exposure", 0.4f);
		shader.m_Shader->SetUniformInt1("u_NoOfLights", lights.size());
		shader.m_Shader->SetUniformFloat("u_RoughnessScale", 0.3f);

		if (texture.m_Textures.size() <= texture.totalTypes)
		{
			for (auto& it : texture.m_Textures)
			{
				if (it.second)
					it.second->Bind(it.first);
			}
		}
	}

	void Renderer::SetupPlane(Camera* camera, const ShaderComponent& shader, const TransformComponent& transform,
		const TextureComponent& texture, const MaterialComponent& material, const std::vector<LightComponent>& lights,
		const Ref<ShadowMap> shadowMap)
	{
		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera->GetProjectionView());
		shader.m_Shader->SetUniformFloat3("u_CameraPosition", camera->GetPosition());

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

		int noOfPointLights = 0;
		bool spotLightExists = false;
		bool dirLightExists = false;
		bool pointLightExists = false;
		for (const auto& l : lights)
		{
			if (l.type == l.LightType::Point)
				noOfPointLights++;

			if (l.type == l.LightType::Spot)
			{
				spotLightExists = true;
			}
			if (l.type == l.LightType::Directional)
			{
				dirLightExists = true;
			}
			if (l.type == l.LightType::Point)
			{
				pointLightExists = true;
			}

		}
		shader.m_Shader->SetUniformInt1("u_NoOfPointLights", noOfPointLights);

		shader.m_Shader->SetUniformInt1("u_SpotLightExists", spotLightExists);
		shader.m_Shader->SetUniformInt1("u_PointLightExists", pointLightExists);
		shader.m_Shader->SetUniformInt1("u_DirLightExists", dirLightExists);

		int i = 0;
		for (const auto& l : lights)
		{
			if (l.type == l.LightType::Directional)
			{
				lightView = glm::lookAt(-l.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
				lightSpaceMatrix = lightProjection * lightView;

				shader.m_Shader->SetUniformFloat3("u_DirLight.u_Direction", l.direction);
				shader.m_Shader->SetUniformFloat3("u_DirLight.u_Ambient",   l.ambient);
				shader.m_Shader->SetUniformFloat3("u_DirLight.u_Diffuse",   l.diffuse);
				shader.m_Shader->SetUniformFloat3("u_DirLight.u_Specular",  l.specular);
			}
			else if (l.type == l.LightType::Point)
			{
				std::string& index = std::to_string(i);
				std::string& s = "u_PointLights[" + index + "].u_Position";
				shader.m_Shader->SetUniformFloat3(s, l.position);

				s = "u_PointLights[" + index + "].u_Ambient";
				shader.m_Shader->SetUniformFloat3(s, l.ambient);

				s = "u_PointLights[" + index + "].u_Diffuse";
				shader.m_Shader->SetUniformFloat3(s, l.diffuse);

				s = "u_PointLights[" + index + "].u_Specular";
				shader.m_Shader->SetUniformFloat3(s, l.specular);

				s = "u_PointLights[" + index + "].u_Constant";
				shader.m_Shader->SetUniformFloat(s, l.constant);

				s = "u_PointLights[" + index + "].u_Linear";
				shader.m_Shader->SetUniformFloat(s, l.linear);

				s = "u_PointLights[" + index + "].u_Quadratic";
				shader.m_Shader->SetUniformFloat(s, l.quadratic);
				
				i++;
			}
			else if (l.type == l.LightType::Spot)
			{
				shader.m_Shader->SetUniformFloat3("u_SpotLight.u_Position", l.direction);
				shader.m_Shader->SetUniformFloat3("u_SpotLight.u_Direction", l.direction);
				shader.m_Shader->SetUniformFloat3("u_SpotLight.u_Ambient", l.ambient);
				shader.m_Shader->SetUniformFloat3("u_SpotLight.u_Diffuse", l.diffuse);
				shader.m_Shader->SetUniformFloat3("u_SpotLight.u_Specular", l.specular);
				shader.m_Shader->SetUniformFloat("u_SpotLight.u_Constant", l.constant);
				shader.m_Shader->SetUniformFloat("u_SpotLight.u_Linear", l.linear);
				shader.m_Shader->SetUniformFloat("u_SpotLight.u_Quadratic", l.quadratic);
				shader.m_Shader->SetUniformFloat("u_SpotLight.u_CutOff", glm::cos(glm::radians(l.cutOff)));
				shader.m_Shader->SetUniformFloat("u_SpotLight.u_OuterCutOff", glm::cos(glm::radians(l.outerCutOff)));
			}
		}

		glm::vec3 p;
		glm::vec3 c;
		if (lights.empty())
		{
			p = glm::vec3(1.0f, 3.0f, 0.0f);
			c = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		else
		{
			p = lights[0].position;
			c = lights[0].ambient;

		}
		//shader.m_Shader->SetUniformFloat3("u_LightPosition", p);
		//shader.m_Shader->SetUniformFloat("u_Shininess", material.m_Material->GetShininess());
		//shader.m_Shader->SetUniformFloat3("u_Specular", material.specular);

		shader.m_Shader->SetUniformFloat3("u_MaterialColor", glm::vec3(1.0f));
		shader.m_Shader->SetUniformMat4("u_LightSpaceMatrix", lightSpaceMatrix);

		for (auto& it : texture.m_Textures)
		{
			if (it.second)
				it.second->Bind(it.first);
			break;
		}

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowMap->GetDepthMapID());
	}

	void Renderer::Setup(Camera* camera, const ShaderComponent& shader,
		const TransformComponent& transform, const TextureComponent& texture, const MaterialComponent& material, 
		const std::vector<LightComponent>& lights)
	{

		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera->GetProjectionView());
		shader.m_Shader->SetUniformFloat3("u_CameraPosition", camera->GetPosition());
		shader.m_Shader->SetUniformInt1("m_DiffuseMap", 0);

		int i = 0;
		int noOfPointLights = 0;
		bool pointLightExists = false, spotLightExits = false, dirLightExists = false;
		for (const auto& l : lights)
		{
			if (l.type == l.LightType::Directional)
			{
				dirLightExists = true;
				shader.m_Shader->SetUniformFloat3("u_DirLight.u_Direction", l.direction);
				shader.m_Shader->SetUniformFloat3("u_DirLight.u_Ambient", l.ambient);
				shader.m_Shader->SetUniformFloat3("u_DirLight.u_Diffuse", l.diffuse);
				shader.m_Shader->SetUniformFloat3("u_DirLight.u_Specular", l.specular);
			}
			else if (l.type == l.LightType::Point)
			{
				pointLightExists = true;
				noOfPointLights++;
				std::string& index = std::to_string(i);
				std::string& s = "u_PointLights[" + index + "].u_Position";
				shader.m_Shader->SetUniformFloat3(s, l.position);

				s = "u_PointLights[" + index + "].u_Ambient";
				shader.m_Shader->SetUniformFloat3(s, l.ambient);

				s = "u_PointLights[" + index + "].u_Diffuse";
				shader.m_Shader->SetUniformFloat3(s, l.diffuse);

				s = "u_PointLights[" + index + "].u_Specular";
				shader.m_Shader->SetUniformFloat3(s, l.specular);

				s = "u_PointLights[" + index + "].u_Constant";
				shader.m_Shader->SetUniformFloat(s, l.constant);

				s = "u_PointLights[" + index + "].u_Linear";
				shader.m_Shader->SetUniformFloat(s, l.linear);

				s = "u_PointLights[" + index + "].u_Quadratic";
				shader.m_Shader->SetUniformFloat(s, l.quadratic);

				i++;
			}
			else if (l.type == l.LightType::Spot)
			{
				spotLightExits = true;
				shader.m_Shader->SetUniformFloat3("u_SpotLight.u_Position", l.direction);
				shader.m_Shader->SetUniformFloat3("u_SpotLight.u_Direction", l.direction);
				shader.m_Shader->SetUniformFloat3("u_SpotLight.u_Ambient", l.ambient);
				shader.m_Shader->SetUniformFloat3("u_SpotLight.u_Diffuse", l.diffuse);
				shader.m_Shader->SetUniformFloat3("u_SpotLight.u_Specular", l.specular);
				shader.m_Shader->SetUniformFloat("u_SpotLight.u_Constant", l.constant);
				shader.m_Shader->SetUniformFloat("u_SpotLight.u_Linear", l.linear);
				shader.m_Shader->SetUniformFloat("u_SpotLight.u_Quadratic", l.quadratic);
				shader.m_Shader->SetUniformFloat("u_SpotLight.u_CutOff", glm::cos(glm::radians(l.cutOff)));
				shader.m_Shader->SetUniformFloat("u_SpotLight.u_OuterCutOff", glm::cos(glm::radians(l.outerCutOff)));
			}
		}

		shader.m_Shader->SetUniformInt1("u_NoOfPointLights", noOfPointLights);
		shader.m_Shader->SetUniformFloat("u_Shininess", 0.4f);
		shader.m_Shader->SetUniformFloat3("u_Specular", glm::vec3(1.0f));
		shader.m_Shader->SetUniformInt1("u_SpotLightExists", (int)spotLightExits);
		shader.m_Shader->SetUniformInt1("u_PointLightExists", (int)pointLightExists);
		shader.m_Shader->SetUniformInt1("u_DirLightExists", (int)dirLightExists);
		shader.m_Shader->SetUniformFloat3("u_CameraPosition", camera->GetPosition());
		//shader.m_Shader->SetUniformFloat3("u_MaterialColor", material.m_Material->GetAlbedoColor());
		shader.m_Shader->SetUniformFloat3("u_MaterialColor", glm::vec3(1.0f));
		
		for (const auto& tex : texture.m_Textures)
		{
			tex.second->Bind();
			break;
		}

	}

	void Renderer::Setup(Camera* camera, const ShaderComponent& shader, const TransformComponent& transform,
		const TextureComponent& texture, const MaterialComponent& material)
	{
		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Transform", transform.GetTransform());
		shader.m_Shader->SetUniformMat4("u_ProjectionView", camera->GetProjectionView());
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

	void Renderer::SetupSkybox(Camera* camera, const SkyboxComponent skybox, const ShaderComponent shader, const glm::mat4& transform)
	{
		shader.m_Shader->Bind();
		shader.m_Shader->SetUniformMat4("u_Projection", camera->GetProjection());
		shader.m_Shader->SetUniformMat4("u_View", glm::mat4(glm::mat3(camera->GetView())));
		shader.m_Shader->SetUniformMat4("u_Model", glm::mat4(1.0f));

		shader.m_Shader->SetUniformInt1("u_EquirectangularMap", 0);

		skybox.m_Skybox->Bind(0);
	}

	void Renderer::SetupCollider(Camera* camera, const Ref<Shader> shader, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetUniformMat4("u_ProjectionView", camera->GetProjectionView());
		shader->SetUniformMat4("u_Model", transform);
	}

	void Renderer::Setup(Camera* camera, const Ref<Shader> shader, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetUniformMat4("u_ProjectionView", camera->GetProjectionView());
		shader->SetUniformMat4("u_Model", transform);
		shader->SetUniformMat3("u_NormalMatrix", glm::mat4(transform));
		shader->SetUniformFloat3("u_CamPos", camera->GetPosition());

		shader->SetUniformInt1("u_Albedo", 0);
		shader->SetUniformInt1("u_MetallicRoughness", 1);
		shader->SetUniformInt1("u_Emissive", 2);
		shader->SetUniformInt1("u_Occlusion", 4);
		shader->SetUniformInt1("u_Normal", 3);
	}

	void Renderer::Render(const Ref<Model> model, const Ref<Shader> shader)
	{
		//if(config.PolygonMode)
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		model->Draw(shader);
		//if(config.PolygonMode) 
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer::RenderCollider(const Ref<Model> model, const Ref<Shader> shader)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		model->Draw(shader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer::Shutdown()
	{

	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewPort(0, 0, width, height);
	}

}