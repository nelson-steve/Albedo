#include "AlbedoPreCompiledHeader.h"

#include "Scene.h"
#include "Entity.h"

#include "Components.h"
#include "Albedo/Utils/AssetSystem.h"
#include "Albedo/Core/Application.h"
#include "Albedo/Renderer/Renderer.h"
#include "Albedo/Physics/PhysicsSolver.h"
#include "Albedo/Physics/PhysicsWorld.h"
#include "Albedo/Scripting/ScriptEngine.h"
#include "Albedo/Physics/Conversions.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Albedo {

	extern std::unique_ptr<AssetSystem> m_AssetManager;


	Scene::Scene()
	{
	}

	void Scene::InitScene()
	{
		m_PhysicsSolver = std::make_shared<PhysicsSolver>();
	
		m_PhysicsSolver->Init();
		m_Collider = m_AssetManager->LoadModel("Assets/models/cube/box.obj");
		m_Collider->GetRendererConfig().Type = DrawType::Albedo_LINE_LOOP;
		m_Shader = m_AssetManager->LoadShader("Assets/Shaders/ModelShader.glsl");

		m_Collider->InitMesh(-1);

		m_SkyboxShader = Shader::Create("Assets/Shaders/Background.glsl");
		m_Skybox = m_AssetManager->LoadDefaultSkybox();
		m_Skybox->InitMesh(-1);

		auto view = m_Registry.view<PhysicsComponent, ColliderComponent, TransformComponent, MaterialComponent>();


		for (auto entity : view)
		{
			auto& mat = view.get<MaterialComponent>(entity);

			mat.m_Material = std::make_shared<Material>();

			auto& tra = view.get<TransformComponent>(entity);
			glm::vec3& pos = tra.Position;
			auto& phy = view.get<PhysicsComponent>(entity);
			phy.physicsMaterial = std::make_shared<PhysicsMaterial>
				(phy.staticFriction, phy.dynamicFriction, phy.restitution);

			if (phy.bodyType == phy.BodyType::Static)
			{
				phy.BodyPosition =  tra.Position;
				phy.staticBody = std::make_shared<RigidBodyStaticComponent>
					(phy.BodyPosition, phy.BodyOrientation);
				//phy.staticBody->GetRigidActor()->setGlobalPose(glmToPhysx::ToTransform(phy.BodyPosition));
			}

			else if (phy.bodyType == phy.BodyType::Dynamic)
			{
				if (phy.infiniteMass)
					phy.Mass = .0f;

				phy.BodyPosition = tra.Position;
				phy.dynamicBody = std::make_shared<RigidBodyDynamicComponent>
					(phy.BodyPosition, phy.BodyOrientation, phy.Mass);
				//phy.dynamicBody->GetRigidActor()->setGlobalPose(glmToPhysx::ToTransform(phy.BodyPosition));
				phy.dynamicBody->DisableGravity(phy.disableGravity);
			}

			auto& col = view.get<ColliderComponent>(entity);
			if (col.colliderType == col.ColliderType::Box)
			{
				if (phy.bodyType == phy.BodyType::Static && phy.staticBody)
				{
					col.ColliderPosition = tra.Position;
					col.collider = std::make_shared<BoxCollider>
						(phy.staticBody.get(), col.ColliderSize,
							phy.physicsMaterial, col.ColliderPosition, col.ColliderOrientation);
					//col.collider->SetType(PhysicsCollider::CollisionType::Collider);
				}

				else if (phy.bodyType == phy.BodyType::Dynamic && phy.dynamicBody)
				{
					col.ColliderPosition = tra.Position;
					col.collider = std::make_shared<BoxCollider>
						(phy.dynamicBody.get(), col.ColliderSize,
							phy.physicsMaterial, col.ColliderPosition, col.ColliderOrientation);
					//col.collider->SetType(PhysicsCollider::CollisionType::Collider);
				}
			}
			else if (col.colliderType == col.ColliderType::Sphere)
			{
				if (phy.bodyType == phy.BodyType::Static && phy.staticBody)
				{
					col.ColliderPosition = tra.Position;
					col.collider = std::make_shared<SphereCollider>
						(phy.staticBody.get(), col.ColliderRadius,
							phy.physicsMaterial, col.ColliderPosition, col.ColliderOrientation);
					//col.collider->SetType(PhysicsCollider::CollisionType::Collider);
				}

				else if (phy.bodyType == phy.BodyType::Dynamic && phy.dynamicBody)
				{
					col.ColliderPosition = tra.Position;
					col.collider = std::make_shared<SphereCollider>
						(phy.dynamicBody.get(), col.ColliderRadius,
							phy.physicsMaterial, col.ColliderPosition, col.ColliderOrientation);
					//col.collider->SetType(PhysicsCollider::CollisionType::Collider);
				}
			}
			else if (col.colliderType == col.ColliderType::ConvexMesh)
			{
				Albedo_Core_ERROR("ConvexMesh not implemented yet");
			}
			else if (col.colliderType == col.ColliderType::Mesh)
			{
				Albedo_Core_ERROR("Mesh not implemented yet");
			}
		}

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = {
			FramebufferTextureFormat::RENDER_BUFFER,
		};
		fbSpec.Width  = 512;
		fbSpec.Height = 512;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		{
			TextureConfiguration config(Config::TextureType::Texture2D, Config::InternalFormat::RGB16F, Config::TextureLayout::ClampToEdge,
				Config::MinMagFilters::LINEAR, Config::MinMagFilters::LINEAR, Config::DataType::FLOAT,
				Config::DataFormat::RGB, false, true);
			config.Path = "Assets/Textures/HDRTexture.hdr";
			hdrTexture = Texture2D::Create(config);
		}
		{
			TextureConfiguration config(Config::TextureType::Cubemap, Config::InternalFormat::RGB16F, Config::TextureLayout::ClampToEdge,
				Config::MinMagFilters::LINEAR_MIPMAP_LINEAR, Config::MinMagFilters::LINEAR, Config::DataType::FLOAT,
				Config::DataFormat::RGB, true, true);
			config.m_Width = 512;
			config.m_Height = 512;
			envCubemap = Texture2D::Create(config);
		}

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
		
		Ref<Shader> shader = Shader::Create("Assets/Shaders/EquirectangulartoCubemap.glsl");
		shader->Bind();
		shader->SetUniformInt1("equirectangularMap", 0);
		shader->SetUniformMat4("projection", captureProjection);
		
		hdrTexture->Bind();
		
		m_Framebuffer->Bind();
		for (unsigned int i = 0; i < 6; ++i)
		{
			shader->SetUniformMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap->GetTextureID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			RendererConfig config;
			m_Collider->GetRendererConfig().Type = DrawType::Albedo_TRIANGLES;
			Renderer::RenderOverlay(m_Collider);
		}
		
		m_Framebuffer->Unbind();
		
		envCubemap->Bind();
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		Renderer::Init(m_Registry);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<MeshComponent>().AddMesh(m_AssetManager->LoadDefaultQuad(), (uint32_t)entity);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<MaterialComponent>().m_Material = std::make_shared<Material>();
		entity.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/Shaders/ModelShader.glsl"));
		entity.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Textures/Diluc.png"));
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::Step(int frames)
	{
		m_StepFrames = frames;
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		//OnUpdatePhysics(ts);

		// Scripting
		{
			ScriptEngine::OnRuntimeStart(this);
			// Instantiate all script entities

			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

		//OnPhysics2DStop();

		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnSimulationStart()
	{
		m_IsSimulating = true;
		//OnUpdatePhysics(ts);
	}

	void Scene::OnSimulationStop()
	{
		m_IsSimulating = false;
	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		Albedo_Core_INFO("simulation");
	}

	void Scene::OnUpdateResize(uint32_t width, uint32_t height)
	{

	}

	void Scene::OnUpdatePhysics(Timestep ts)
	{
		m_PhysicsSolver->UpdatePhysics(ts);

		auto view = m_Registry.view<TransformComponent, PhysicsComponent, ColliderComponent>();


		for (auto entity : view)
		{
			auto& phy = view.get<PhysicsComponent>(entity);
			auto& col = view.get<ColliderComponent>(entity);
			auto& tra = view.get<TransformComponent>(entity);

			if (phy.bodyType == phy.BodyType::Dynamic && phy.dynamicBody)
			{
				auto& p = phy.dynamicBody->GetRigidActor()->getGlobalPose().p;
				auto& q = phy.dynamicBody->GetRigidActor()->getGlobalPose().q;
				Albedo_Core_INFO("Rotation: {} {} {} {}", q.x, q.y, q.z, q.w);
				tra.AddTranform(glm::vec3(p.x, p.y, p.z), glm::vec4(q.x, q.y, q.z, q.w), tra.Scale);

				phy.BodyPosition = glm::vec3(p.x, p.y, p.z);
				phy.BodyOrientation = glm::quat(q.x, q.y, q.z, q.w);
			}
			else if (phy.bodyType == phy.BodyType::Static && phy.staticBody)
			{
				auto p = phy.staticBody->GetRigidActor()->getGlobalPose().p;
				tra.AddTranform(glm::vec3(p.x, p.y, p.z));
				phy.BodyPosition = glm::vec3(p.x, p.y, p.z);
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
			ScriptEngine::OnRuntimeStart(this);
			// Instantiate all script entities

			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}

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
					mainCamera->SetPosition(transform.Position);
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
				auto& shader = view.get<ShaderComponent>(entity);
				if (mesh.m_Mesh->GetInitializationStatus() || shader.m_Shader->GetInitializationStatus())
					InitScene();
			}
			for (auto entity : view)
			{
				if (m_Registry.any_of<CameraComponent>(entity)) continue;

				// TODO: fix
				//Renderer::Setup(*mainCamera, view.get<ShaderComponent>(entity), view.get<TransformComponent>(entity), view.get<TextureComponent>(entity));
				//Renderer::Render(view.get<MeshComponent>(entity), view.get<MeshComponent>(entity).m_Mesh->GetRendererConfig());
			}
		}
	}

	void Scene::OnUpdateEditor(EditorCamera& camera, Timestep ts)
	{
		GLint drawFboId;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &drawFboId);
		Albedo_Core_INFO("framebuffer DURING: {}", drawFboId);

		auto view = m_Registry.view<PhysicsComponent, ColliderComponent, ShaderComponent, TransformComponent, MeshComponent,
			TextureComponent, MaterialComponent>();
		
		for (auto& entity : view)
		{
			auto& mesh = view.get<MeshComponent>(entity);
			auto& shader = view.get<ShaderComponent>(entity);
			if (mesh.m_Mesh->GetInitializationStatus() || shader.m_Shader->GetInitializationStatus())
				InitScene();
		}

		if (m_IsSimulating)
		{
			OnUpdatePhysics(ts);

			//for (auto& entity : view)
			//{
			//	auto& pos = view.get<PhysicsComponent>(entity).BodyPosition;
			//	view.get<TransformComponent>(entity).Position = pos;
			//}

			for (auto& entity : view)
			{
				Renderer::Setup(camera, (view.get<ShaderComponent>(entity)), view.get<TransformComponent>(entity),
					view.get<TextureComponent>(entity), view.get<MaterialComponent>(entity));
				Renderer::Render(view.get<MeshComponent>(entity), view.get<MeshComponent>(entity).m_Mesh->GetRendererConfig());
			}
		}
		else
		{
			for (auto& entity : view)
			{
				auto& pos = view.get<TransformComponent>(entity).Position;
				auto& rot = view.get<TransformComponent>(entity).Rotation;
				view.get<PhysicsComponent>(entity).BodyPosition = pos;
				view.get<PhysicsComponent>(entity).BodyOrientation = glm::quat(rot);
				view.get<ColliderComponent>(entity).ColliderPosition = pos;
			}

			for (auto& entity : view)
			{
				Renderer::Setup(camera, (view.get<ShaderComponent>(entity)), view.get<TransformComponent>(entity),
					view.get<TextureComponent>(entity), view.get<MaterialComponent>(entity));
				Renderer::Render(view.get<MeshComponent>(entity), view.get<MeshComponent>(entity).m_Mesh->GetRendererConfig());
			}

			for (auto& entity : view)
			{
				auto& phy = view.get<PhysicsComponent>(entity);
				auto& col = view.get<ColliderComponent>(entity);

				m_Transform = glm::translate(glm::mat4(1.0f), phy.BodyPosition) * glm::scale(glm::mat4(1.0f), col.ColliderSize);
				//glm::vec3 offset = glm::vec3(2.0);
				//m_Transform = glm::scale(m_Transform, offset);
				{
					//Renderer::Setup(camera, m_Shader, m_Transform);
					//Renderer::RenderOverlay(m_Collider);
				}
			}
		}

		m_SkyboxShader->Bind();
		m_SkyboxShader->SetUniformMat4("projection", camera.GetProjection());
		m_SkyboxShader->SetUniformMat4("view", camera.GetViewMatrix());
		m_SkyboxShader->SetUniformInt1("environmentMap", 0);

		glDepthMask(GL_FALSE);
		m_Skybox->GetMeshBufferData().m_VertexArray->Bind();
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap->GetTextureID());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		m_Skybox->GetMeshBufferData().m_VertexArray->UnBind();
		glDepthMask(GL_TRUE);
		//Renderer::RenderOverlay(m_Skybox);
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;

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

	Entity Scene::FindEntityByName(std::string_view name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
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
	void Scene::OnComponentAdded<ColliderComponent>(Entity entity, ColliderComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
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

	template<>
	void Scene::OnComponentAdded<PhysicsComponent>(Entity entity, PhysicsComponent& component)
	{
	}

}