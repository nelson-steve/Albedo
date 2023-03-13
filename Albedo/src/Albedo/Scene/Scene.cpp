#include "AlbedoPreCompiledHeader.h"

#include "Scene.h"
#include "Entity.h"

#include "Components.h"
#include "Albedo/Renderer/Renderer2D.h"
#include "Albedo/Renderer/Renderer.h"

#include <glm/glm.hpp>

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

	void Scene::OnUpdateEditor(EditorCamera& camera)
	{
		//Renderer3D::BeginScene(camera);
		//for (Material* material : m_Materials)
		for(Material* material: m_Materials)
		{
			Renderer::Setup(camera, (*material));
			Renderer::Render(*material);
		}
		//Renderer3D::RenderSkybox();
		//Renderer3D::EndScene(camera);

		//begin scene;
		//for (begin end)
		//{
		//	materialSystem::CurrnentComponent().intantiate
		//}

#if 0
		Renderer3D::BeginScene(camera);
		//Renderer3D::DrawCube({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		//Renderer3D::DrawCube({ 0.0f, 0.0f, -5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
		float value = 5.0f;
		for (float y = -value; y < value; y += 0.5f)
		{
			for (float x = -value; x < value; x += 0.5f)
			{
				//Renderer3D::DrawCube({ x, 0.0f, y }, { 0.2f, 0.2f, 0.2f }, { (x / value + 1) / 2 , (y / value + 1) / 2, (y / value + 1) / 2, 1.0f });
				//Renderer3D::DrawCube({ x, y, 1.0f }, { 0.2f, 0.2f, 0.2f }, { (y / value + 1) / 2 , (x / value + 1) / 2, (y / value + 1) / 2, 1.0f });
				//Renderer3D::DrawCube({ x, y, 2.0f }, { 0.2f, 0.2f, 0.2f }, { (x / value + 1) / 2 , (y / value + 1) / 2, (x / value + 1) / 2, 1.0f });
			}
		}
		//Renderer3D::DrawCube({ 0.0f, -5.0f, 0.0f }, { 100.0f, 1.0f, 100.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
		Renderer3D::DrawModel();
		Renderer3D::EndScene(camera);

		//Renderer3D::Init();
		//Renderer3D::BeginScene(camera);
		//Renderer3D::DrawCube({ 5.0f, 5.0f, 0.0f }, { 0.5f, 0.2f, 0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f });
		//Renderer3D::EndScene();
#endif
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
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