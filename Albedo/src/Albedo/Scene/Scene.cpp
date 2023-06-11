#include "AlbedoPreCompiledHeader.h"

#include "Scene.h"
#include "Entity.h"
#include "imgui.h"

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
		//glEnable(GL_DEPTH_TEST);
		//
		//// set depth function to less than AND equal for skybox depth trick.
		//glDepthFunc(GL_LEQUAL);
		//// enable seamless cubemap sampling for lower mip levels in the pre-filter map.
		//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		m_PhysicsSolver = std::make_shared<PhysicsSolver>();
	
		tex = m_AssetManager->LoadTexture("Assets/Textures/Wood.png", true);
		m_PhysicsSolver->Init();
		m_Collider = m_AssetManager->LoadModelusingAssimp("Assets/models/cube/box.obj");
		m_Cube = m_AssetManager->LoadModelusingAssimp("Assets/Models/suzanne/suzanne.obj");
		m_Quad = m_AssetManager->LoadModelusingAssimp("Assets/Models/plane/plane2.obj");
		m_Collider->GetRendererConfig().Type = DrawType::Albedo_LINE_LOOP;
		m_Shader = m_AssetManager->LoadShader("Assets/Shaders/ModelShader.glsl");

		m_Collider->InitMesh(-1);
		m_Quad->InitMesh(-1);
		m_Cube->InitMesh(-1);

		m_SkyboxShader = Shader::Create("Assets/Shaders/Background.glsl");
		m_DepthShader  = Shader::Create("Assets/Shaders/DepthMapShader.glsl");
		//m_IrradianceShader = Shader::Create("Assets/Shaders/IrradiancetoConvolution.glsl");
		//m_PrefilterShader = Shader::Create("Assets/Shaders/Prefilter.glsl");
		//m_brdfShader = Shader::Create("Assets/Shaders/BRDF.glsl");
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

		{
			TextureConfiguration config(Config::TextureType::Cubemap, Config::InternalFormat::RGB, Config::TextureLayout::ClampToEdge,
				Config::MinMagFilters::LINEAR, Config::MinMagFilters::LINEAR, Config::DataType::UNSIGNED_BYTE,
				Config::DataFormat::RGB, false, false);
			config.Faces =
			{
				"Assets/Textures/Skybox/right.jpg",
				"Assets/Textures/Skybox/left.jpg",
				"Assets/Textures/Skybox/top.jpg",
				"Assets/Textures/Skybox/bottom.jpg",
				"Assets/Textures/Skybox/front.jpg",
				"Assets/Textures/Skybox/back.jpg"
			};
			skyboxTemp = Texture2D::Create(config);
		}

		m_ShadowMap = std::make_shared<ShadowMap>(2048, 2048);

		if (fbo)
		{
			//FramebufferSpecification fbSpec;
			//fbSpec.Attachments = {
			//	FramebufferTextureFormat::Depth
			//};
			//fbSpec.Width = 1024;
			//fbSpec.Height = 1024;
			//fbSpec.s_BorderColor = true;
			//fbSpec.s_DrawBuffer = false;
			//fbSpec.s_ReadBuffer = false;
			//
			//TextureConfiguration config(Config::TextureType::Texture2D, Config::InternalFormat::DEPTH, Config::TextureLayout::ClampToBorder,
			//	Config::MinMagFilters::NEAREST, Config::MinMagFilters::NEAREST, Config::DataType::FLOAT,
			//	Config::DataFormat::DEPTH, true, false);
			//m_DepthMapFBO = Framebuffer::Create(fbSpec, config);
			//fbo = false;
		}

		Renderer::Init(m_Registry);
	}

	Entity Scene::CreateMeshEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<MeshComponent>().AddMesh(m_AssetManager->LoadDefaultQuad(), (uint32_t)entity);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<MaterialComponent>().m_Material = std::make_shared<Material>();
		entity.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/Shaders/ModelPBRShader.glsl"));
		entity.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Textures/Diluc.png"));
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	Entity Scene::CreateLightEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<LightComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	Entity Scene::CreateSkyboxEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<SkyboxComponent>();
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
		auto view = m_Registry.view<PhysicsComponent, ColliderComponent, ShaderComponent, TransformComponent, MeshComponent,
			TextureComponent, MaterialComponent>();

		// Checking for re initialization of meshes
		for (auto& entity : view)
		{
			auto& mesh = view.get<MeshComponent>(entity);
			auto& shader = view.get<ShaderComponent>(entity);
			if (mesh.m_Mesh->GetInitializationStatus() || shader.m_Shader->GetInitializationStatus())
				InitScene();
		}

		// Getting light components
		auto& lightComponents = m_Registry.view<LightComponent>();
		std::vector<LightComponent> lights;
		glm::vec3 lightDirection{ 0.0 };
		for (auto& entity : lightComponents)
		{
			auto& dirLight = lightComponents.get<LightComponent>(entity);
			if (dirLight.type == dirLight.LightType::Directional)
			{
				lightDirection = lightComponents.get<LightComponent>(entity).direction;
			}
			lights.push_back(lightComponents.get<LightComponent>(entity));
		}
		glm::vec3 l;
		if (lights.empty())
			l = glm::vec3(0.0f, 5.0f, 0.0f);
		else
			l = lights[0].position;

		//
		// Shadow map render pass
		//
		Renderer::PreRenderPass(m_DepthShader, m_ShadowMap, m_Registry, lightDirection, tex);

		m_Framebuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (m_IsSimulating)
			OnUpdatePhysics(ts);

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
			if (view.get<MaterialComponent>(entity).m_Material->IsPBR())
			{
				Renderer::SetupPBR(camera, (view.get<ShaderComponent>(entity)), view.get<TransformComponent>(entity),
					view.get<TextureComponent>(entity), view.get<MaterialComponent>(entity), lights);
			}
			else if (!view.get<MaterialComponent>(entity).m_Material->IsPBR())
			{
				Renderer::SetupPlane(camera, (view.get<ShaderComponent>(entity)), view.get<TransformComponent>(entity),
					view.get<TextureComponent>(entity), view.get<MaterialComponent>(entity), lights, m_ShadowMap);
			}

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

		m_SkyboxShader->Bind();
		m_SkyboxShader->SetUniformMat4("projection", camera.GetProjection());
		m_SkyboxShader->SetUniformMat4("view", camera.GetViewMatrix());
		m_SkyboxShader->SetUniformInt1("environmentMap", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTemp->GetTextureID());

		glDepthMask(GL_FALSE);
		m_Skybox->GetMeshBufferData().m_VertexArray->Bind();
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
	void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SkyboxComponent>(Entity entity, SkyboxComponent& component)
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