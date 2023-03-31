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

	Scene::Scene()
	{

	}

	Scene::~Scene()
	{
	}

	void Scene::InitScene()
	{
		//Renderer::InitSkybox();

		Renderer::Init(m_Registry);
		//Renderer::Init(m_SceneObjects);
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
				//auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				//if (camera.Primary)
				{
					//mainCamera = &camera.Camera;
					//cameraTransform = transform.GetTransform();
					//break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			//auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			//for (auto entity : group)
			//{
			//	auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			//
			//	Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			//}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateEditor(EditorCamera& camera, Timestep ts)
	{
		//for (auto scnObj : m_SceneObjects)
		//{	
		//	Renderer::Setup(camera, scnObj);
		//	Renderer::Render(scnObj);
		//}

		auto view = m_Registry.view<ShaderComponent, TransformComponent, MeshComponent>();

		for (auto group : view)
		{
			auto& shader = view.get<ShaderComponent>(group);
			auto& transform = view.get<TransformComponent>(group);
			auto& mesh = view.get<MeshComponent>(group);


			Renderer::Setup(camera, shader, transform);
			Renderer::Render(mesh);

		}
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
		Albedo_Core_INFO("nothing");
		//static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
		Albedo_Core_INFO("nothing");
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

	template<>
	void Scene::OnComponentAdded<ShaderComponent>(Entity entity, ShaderComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TextureComponent>(Entity entity, TextureComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
	}
}