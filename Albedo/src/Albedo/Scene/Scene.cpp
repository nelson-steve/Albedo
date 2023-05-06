#include "AlbedoPreCompiledHeader.h"

#include "Scene.h"
#include "Entity.h"

#include "Components.h"
#include "Albedo/Utils/AssetSystem.h"
#include "Albedo/Core/Application.h"
#include "Albedo/Renderer/Renderer.h"
#include "Albedo/Physics/PhysicsSolver.h"
#include "Albedo/Physics/PhysicsWorld.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Albedo {

	extern std::unique_ptr<AssetSystem> m_AssetManager;

	//Ref<PhysicsWorld> m_PhysicsWorld;

	Scene::Scene()
	{
		m_PhysicsSolver = std::make_shared<PhysicsSolver>();
		//m_PhysicsWorld = std::make_shared<PhysicsWorld>();
	}

	Scene::~Scene()
	{
		m_PhysicsSolver->Deallocate();
		//m_PhysicsWorld.reset();
	}

	void Scene::InitScene()
	{
		//m_PhysicsWorld->Init();

		m_Collider = m_AssetManager->LoadModel("Assets/models/cube/box.obj");
		m_Collider->GetRendererConfig().Type = DrawType::Albedo_LINE_LOOP;
		m_Shader = m_AssetManager->LoadShader("Assets/ModelShader.glsl");

		m_Collider->InitMesh(-1);

		auto view = m_Registry.view<PhysicsComponent>();

		for (auto entity : view)
		{
			view.get<PhysicsComponent>(entity).Create();
		}

		Renderer::Init(m_Registry);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<MeshComponent>().AddMesh(m_AssetManager->LoadDefaultQuad(), (uint32_t)entity);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/ModelShader.glsl"));
		entity.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/models/fa/Diffuse.jpg"));
		//entity.AddComponent<ColliderComponent>().collider = std::make_shared<BoxCollider>();
		//entity.GetComponent<ColliderComponent>().collider->SetType(Type::BoxAABB);
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
		//m_PhysicsWorld->Update(ts);
		m_PhysicsSolver->UpdatePhysics(ts);

		auto view = m_Registry.view<TransformComponent, PhysicsComponent>();

		for (auto entity : view)
		{

			if (view.get<PhysicsComponent>(entity)._BodyType == view.get<PhysicsComponent>(entity).BodyType::Dynamic)
			{
				auto p = view.get<PhysicsComponent>(entity).dynamicBody->GetRigidActor()->getGlobalPose().p;
				view.get<TransformComponent>(entity).AddTranform(glm::vec3(p.x, p.y, p.z));
			}
			else if (view.get<PhysicsComponent>(entity)._BodyType == view.get<PhysicsComponent>(entity).BodyType::Static)
			{
				auto p = view.get<PhysicsComponent>(entity).staticBody->GetRigidActor()->getGlobalPose().p;
				view.get<TransformComponent>(entity).AddTranform(glm::vec3(p.x, p.y, p.z));
			}
			else
				Albedo_Core_ERROR("Invalid Body Type");
		}

		//m_PhysicsSolver->Update(ts);
		//
		//// retrieve array of actors that moved
		//physx::PxU32 nbActiveActors;
		//physx::PxActor** activeActors = m_PhysicsSolver->scene->getActiveActors(nbActiveActors);
		//if (activeActors == nullptr || nbActiveActors == 0) return;
		//const physx::PxRigidActor* s = dynamic_cast<physx::PxRigidActor*>(*activeActors);
		//
		//// update each render object with the new transform
		//for (physx::PxU32 i = 0; i < nbActiveActors; ++i)
		//{
		//	Mesh* renderObject = static_cast<Mesh*>(activeActors[i]->userData);
		//	const auto& t = s[i].getGlobalPose().p;
		//	Albedo_Core_INFO("pos: {0} {1} {2}", t.x, t.y, t.z);
		//}
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
				if (m_Registry.any_of<CameraComponent>(entity)) continue;

				Renderer::Setup(*mainCamera, view.get<ShaderComponent>(entity), view.get<TransformComponent>(entity), view.get<TextureComponent>(entity));
				Renderer::Render(view.get<MeshComponent>(entity), view.get<MeshComponent>(entity).m_Mesh->GetRendererConfig());
			}
		}
	}

	void Scene::OnUpdateEditor(EditorCamera& camera, Timestep ts)
	{
		OnUpdatePhysics(ts);

		auto view = m_Registry.view<PhysicsComponent, ShaderComponent, TransformComponent, MeshComponent, TextureComponent>();

		for (auto& entity : view)
		{
			auto& mesh = view.get<MeshComponent>(entity);
			if (mesh.m_Mesh->GetInitializationStatus())
				InitScene();
		}

		for (auto& entity : view)
		{
			Renderer::Setup(camera, (view.get<ShaderComponent>(entity)), view.get<TransformComponent>(entity), view.get<TextureComponent>(entity));
			Renderer::Render(view.get<MeshComponent>(entity), view.get<MeshComponent>(entity).m_Mesh->GetRendererConfig());
		}

		for (auto& entity : view)
		{
			auto& phy = view.get<PhysicsComponent>(entity);

			m_Transform = glm::translate(glm::mat4(1.0f), phy.ColliderPosition) * glm::scale(glm::mat4(1.0f), phy.ColliderSize);
			glm::vec3 offset = glm::vec3(2.0);
			m_Transform = glm::scale(m_Transform, offset);
			{
				Renderer::Setup(camera, m_Shader, m_Transform);
				Renderer::RenderOverlay(m_Collider);
			}
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
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
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
		Albedo_Core_INFO("nothing");
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
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

}