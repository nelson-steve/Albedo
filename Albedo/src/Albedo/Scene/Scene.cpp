#include "AlbedoPreCompiledHeader.h"

#include "Scene.h"
#include "Entity.h"

#include "Albedo/Physics/PhysicsWorld.h"
#include "Components.h"
#include "Albedo/Renderer/Renderer2D.h"
#include "Albedo/Renderer/Renderer.h"

#include "Albedo/Utils/AssetSystem.h"
#include "Albedo/Core/Application.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define ENTT_EXAMPLE_CODE 0

namespace Albedo {

	extern std::unique_ptr<Albedo::AssetSystem> m_AssetManager;

	Ref<PhysicsWorld> m_PhysicsWorld;

	Scene::Scene()
	{
		m_PhysicsWorld = std::make_shared<PhysicsWorld>();
	}

	Scene::~Scene()
	{

	}

	void Scene::InitScene()
	{
		m_Collider = m_AssetManager->LoadDefaultCircle();
		m_Collider->GetRendererConfig().Type = DrawType::Albedo_LINE_LOOP;
		m_Shader = m_AssetManager->LoadShader("Assets/ModelShader.glsl");

		m_Collider->InitMesh(-1);

		Renderer::Init(m_Registry);

		auto view = m_Registry.view<ColliderComponent>();
		for (auto entity : view)
		{
			bool exists = false;
			for (auto c : m_PhysicsWorld->GetColliderList())
			{
				if (c == view.get<ColliderComponent>(entity).collider)
					exists = true;
			}
			if(!exists)
				m_PhysicsWorld->GetColliderList().push_back(view.get<ColliderComponent>(entity).collider);
		}
		//Renderer::InitSkybox();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };	
		entity.AddComponent<MeshComponent>().AddMesh(m_AssetManager->LoadDefaultQuad(), (uint32_t)entity);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/ModelShader.glsl"));
		entity.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/models/fa/Diffuse.jpg"));
		entity.AddComponent<ColliderComponent>().collider = std::make_shared<SphereCollider>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdatePhysics(Timestep ts)
	{
		//m_PhysicsWorld->Update();

		{
			//collision update
			auto view = m_Registry.view<ColliderComponent>();

		}

		{
			//position update
			auto view = m_Registry.view<PhysicsComponent, TransformComponent, ColliderComponent>();
			for (auto entity : view)
			{
				if (view.get<PhysicsComponent>(entity).physicsEnabled)
					m_PhysicsWorld->Update(ts, view.get<PhysicsComponent>(entity), view.get<TransformComponent>(entity), view.get<ColliderComponent>(entity));
			}
		}
	
		//validation
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		OnUpdatePhysics(ts);
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

		SceneCamera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					mainCamera->SetPosition(transform.GetPosition());
					break;
				}
			}
		}

		if (mainCamera)
		{	
			auto view = m_Registry.view<ShaderComponent, TransformComponent, MeshComponent, TextureComponent>();

			for (auto entity : view)
			{
				auto& mesh = view.get<MeshComponent>(entity);
				if (mesh.m_Mesh->GetInitializationStatus())
					InitScene();
			}
			for (auto entity : view)
			{
				if(m_Registry.any_of<CameraComponent>(entity)) continue;

				Renderer::Setup(*mainCamera, view.get<ShaderComponent>(entity), view.get<TransformComponent>(entity), view.get<TextureComponent>(entity));
				Renderer::Render(view.get<MeshComponent>(entity), view.get<MeshComponent>(entity).m_Mesh->GetRendererConfig());
			}
		}
	}

	void Scene::OnUpdateEditor(EditorCamera& camera, Timestep ts)
	{
		auto view = m_Registry.view<ShaderComponent, TransformComponent, MeshComponent, TextureComponent, ColliderComponent>();

		for (auto& entity : view)
		{
			auto& mesh = view.get<MeshComponent>(entity);
			if (mesh.m_Mesh->GetInitializationStatus())
				InitScene();
				//mesh.m_Mesh->InitMesh(view.get<MeshComponent>(entity).ID);
		}

		for (auto& entity : view)
		{
			Renderer::Setup(camera, (view.get<ShaderComponent>(entity)), view.get<TransformComponent>(entity), view.get<TextureComponent>(entity));
			Renderer::Render(view.get<MeshComponent>(entity), view.get<MeshComponent>(entity).m_Mesh->GetRendererConfig());
		}

		for (auto& entity : view)
		{
			auto& collider = view.get<ColliderComponent>(entity);
			auto& transform = view.get<TransformComponent>(entity);
			std::dynamic_pointer_cast<SphereCollider>(collider.collider)->SetCenter(transform.GetPosition());
			glm::mat4& m_Transform = glm::translate(glm::mat4(1.0), collider.collider->GetCenter()) *
				glm::scale(glm::mat4(1.0), (collider.collider->GetRadius() * glm::vec3(1.0)));
			
			if (view.get<ColliderComponent>(entity).ShowCollider)
			{
				Renderer::Setup(camera, m_Shader, m_Transform);
				Renderer::RenderOverlay(m_Collider);
			}
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
		Albedo_Core_INFO("invalid type");
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
		//component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
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
		Albedo_Core_INFO("mesh added");
	}

	template<>
	void Scene::OnComponentAdded<PhysicsComponent>(Entity entity, PhysicsComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ColliderComponent>(Entity entity, ColliderComponent& component)
	{
	}
}