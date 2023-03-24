#include "AlbedoPreCompiledHeader.h"

#include "Scene.h"
#include "Entity.h"

#include "Components.h"
#include "Albedo/Renderer/Renderer2D.h"
#include "Albedo/Renderer/Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define ENTT_EXAMPLE_CODE 0

namespace Albedo {

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	Scene::Scene()
	{

	}

	Scene::~Scene()
	{
		for (Material* material : m_Materials)
		{
			delete material;
		}
	}

	void Scene::InitScene()
	{
		Renderer::InitMaterials(m_Materials);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		// Update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_Entity = Entity{ entity, this };

						nsc.Instance->OnCreate();
					}

					nsc.Instance->OnUpdate(ts);
				});
		}

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
					}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateEditor(EditorCamera& camera, Timestep ts)
	{
		Renderer::DrawModel(camera);
		glm::mat4 transform = glm::mat4(1.0f);
		glm::vec3 color(1.0, 0.0, 0.0);
		for(Material* material: m_Materials)
		{	
#if 0
			Renderer::PreRenderSetup(camera, *material);
			Renderer::PreRenderShader->Bind();
			
			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(0.0f, 1.0f, 0.0));
			transform = glm::scale(transform, glm::vec3(2.0f));
			Renderer::PreRenderShader->SetUniformMat4("u_Transform", transform);
			Renderer::PreRenderRender(*material);

			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(2.5f, 4.0f, 0.0));
			transform = glm::scale(transform, glm::vec3(1.0f));
			Renderer::PreRenderShader->SetUniformMat4("u_Transform", transform);
			Renderer::PreRenderRender(*material);
			
			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(0.0f, -3.0f, 0.0));
			//transform = glm::rotate(transform, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			transform = glm::scale(transform, glm::vec3(20.0, 1.0, 20.0));
			Renderer::PreRenderShader->SetUniformMat4("u_Transform", transform);
			Renderer::PreRenderRender(*material);

			//Renderer::PlaneRender(*material);

			Renderer::Reset();
#endif
#if 1
			//Renderer::DebugRender();
#else
			Renderer::Setup(camera, *material);

			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(0.0f, 1.0f, 0.0));
			transform = glm::scale(transform, glm::vec3(2.0f));
			material->GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);
			Renderer::Render(*material);

			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(2.5f, 4.0f, 0.0));
			transform = glm::scale(transform, glm::vec3(1.0f));
			material->GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);
			Renderer::Render(*material);

			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(0.0f, -3.0f, 2.0));
			//transform = glm::rotate(transform, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			transform = glm::scale(transform, glm::vec3(20.0, 1.0, 20.0));
			material->GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);
			Renderer::Render(*material);

			//Renderer::PlaneRender(*material);
#endif

#if 0
			glm::vec3 pos;
			if (material->GetMaterialType() == MaterialType::Light && material->Show())
			{
				for (uint32_t i = 0; i < 4; i++)
				{
					Albedo_Core_INFO("Scene: Light");
						 if (i == 0) pos = material->GetMaterialData().PointLightPos1;
					else if (i == 1) pos = material->GetMaterialData().PointLightPos2;
					else if (i == 2) pos = material->GetMaterialData().PointLightPos3;
					else if (i == 3) pos = material->GetMaterialData().PointLightPos4;
	
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
						* glm::scale(glm::mat4(1.0f), material->GetMaterialData().Scale);
					material->GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);

					Renderer::Setup(camera, (*material));
					Renderer::Render(*material);
				}
			}

			else if(material->GetMaterialType() == MaterialType::Cube && material->Show())
			{
				for (uint32_t i = 0; i < 10; i++)
				{
					Albedo_Core_INFO("Scene: Cube");
					// calculate the model matrix for each object and pass it to shader before drawing
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), cubePositions[i])
						* glm::scale(glm::mat4(1.0f), material->GetMaterialData().Scale);
					float angle = material->GetMaterialData().Angle * i;
					transform = glm::rotate(transform, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
					material->GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);

					Renderer::Setup(camera, (*material));
					Renderer::Render(*material);
				}
			}
			else if(material->GetMaterialType() == MaterialType::Skybox && material->Show())
			{
				Renderer::Setup(camera, (*material));
				Renderer::Render(*material);
			}
#endif
		}

#if 0
		if (material->GetMaterialType() == MaterialType::Cube)
		{
			float value = 100.0f;
			for (float y = -value; y < value; y += 10.0f)
			{
				for (float x = -value; x < value; x += 10.0f)
				{
					material->ChangePosition(glm::vec3(x, 0.0, y));

					Renderer::Setup(camera, (*material));
					Renderer::Render(*material);
				}
			}
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

		for (auto entity : group)
		{
			//auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
		}
#endif
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth  = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}

	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
}