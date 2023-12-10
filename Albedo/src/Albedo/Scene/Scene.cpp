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

#include <box2d/box2d.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define ALBEDO_PHYSX 0;

namespace Albedo {

	extern std::unique_ptr<AssetSystem> m_AssetManager;

	static b2BodyType Rigidbody2DTypeToBox2DBody(Physics2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Physics2DComponent::BodyType::Static:    return b2_staticBody;
			case Physics2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
			case Physics2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		Albedo_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	Scene::Scene()
	{
		m_AssetManager->LoadNullTexture();
		//TODO: Remove this if not needed
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
			{
				if (src.HasComponent<Component>())
					dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	void Scene::InitDefaults()
	{
		tex = m_AssetManager->LoadTexture("Assets/Textures/Wood.png", true);
		//m_Collider = m_AssetManager->LoadModelusingAssimp("Assets/models/cube/box.obj");
		//m_Cube = m_AssetManager->LoadModelusingAssimp("Assets/Models/suzanne/suzanne.obj");
		//m_Quad = m_AssetManager->LoadModelusingAssimp("Assets/Models/plane/plane2.obj");
		//m_Collider->GetRendererConfig().Type = DrawType::Albedo_LINE_LOOP;
		//m_ColliderShader = m_AssetManager->LoadShader("Assets/Shaders/ColliderShader.glsl");

		//m_Collider->InitMesh(-1);
		//m_Quad->InitMesh(-1);
		//m_Cube->InitMesh(-1);

		m_SkyboxShader = Shader::Create("Assets/Shaders/Background.glsl");
		m_DepthShader = Shader::Create("Assets/Shaders/DepthMapShader.glsl");
		//m_Skybox = m_AssetManager->LoadDefaultSkybox();
		//m_Skybox->InitMesh(-1);

#if ALBEDO_PHYSX
		m_PhysicsSolver = std::make_shared<PhysicsSolver>();
		m_PhysicsSolver->Init();
#endif
		{
			TextureConfiguration config(Config::TextureType::Cubemap, Config::InternalFormat::RGB, Config::TextureLayout::ClampToEdge,
				Config::MinMagFilters::LINEAR, Config::MinMagFilters::LINEAR, Config::DataType::UNSIGNED_BYTE,
				Config::DataFormat::RGB, false, false);
			//config.Faces =
			//{
			//	"Assets/Textures/Skybox/lake/right.jpg",
			//	"Assets/Textures/Skybox/lake/left.jpg",
			//	"Assets/Textures/Skybox/lake/top.jpg",
			//	"Assets/Textures/Skybox/lake/bottom.jpg",
			//	"Assets/Textures/Skybox/lake/front.jpg",
			//	"Assets/Textures/Skybox/lake/back.jpg"
			//};
			//skyboxTemp = Texture2D::Create(config);
		}
 
		m_ShadowMap = std::make_shared<ShadowMap>(2048, 2048);

		m_DefaultsInitialized = true;
	}

	void Scene::InitPhysicsObjects()
	{
		auto view = m_Registry.view<PhysicsComponent, ColliderComponent, TransformComponent, MaterialComponent>();

		for (auto entity : view)
		{
			auto& tra = view.get<TransformComponent>(entity);
			auto& phy = view.get<PhysicsComponent>(entity);
			auto& col = view.get<ColliderComponent>(entity);
			phy.initialize = true;
			col.initialize = true;
			
			phy.physicsMaterial = std::make_shared<PhysicsMaterial>
				(phy.staticFriction, phy.dynamicFriction, phy.restitution);

			if (phy.bodyType == phy.BodyType::Static && phy.initialize)
			{
				phy.BodyPosition = tra.Position;
				phy.staticBody = std::make_shared<RigidBodyStaticComponent>
					(phy.BodyPosition, phy.BodyOrientation);
				phy.initialize = false;
				phy.staticBody->GetRigidActor()->setGlobalPose(glmToPhysx::ToTransform(phy.BodyPosition));
			}

			else if (phy.bodyType == phy.BodyType::Dynamic && phy.initialize)
			{
				//if (phy.infiniteMass)
				//	phy.Mass = .0f;

				phy.BodyPosition = tra.Position;
				phy.dynamicBody = std::make_shared<RigidBodyDynamicComponent>
					(phy.BodyPosition, phy.BodyOrientation, phy.Mass);
				phy.dynamicBody->GetRigidActor()->setGlobalPose(glmToPhysx::ToTransform(phy.BodyPosition));
				phy.dynamicBody->DisableGravity(phy.disableGravity);
				if(phy.isKinematic)
					phy.dynamicBody->MakeKinematic();
				phy.initialize = false;
			}

			if (col.colliderType == col.ColliderType::Box)
			{
				if (phy.bodyType == phy.BodyType::Static && phy.staticBody && col.initialize)
				{
					col.ColliderPosition = tra.Position;
					col.collider = std::make_shared<BoxCollider>
						(phy.staticBody.get(), col.ColliderSize,
							phy.physicsMaterial, col.ColliderPosition, col.ColliderOrientation);
					col.initialize = false;
					//col.collider->SetType(PhysicsCollider::CollisionType::Collider);
				}

				else if (phy.bodyType == phy.BodyType::Dynamic && phy.dynamicBody && col.initialize)
				{
					col.ColliderPosition = tra.Position;
					col.collider = std::make_shared<BoxCollider>
						(phy.dynamicBody.get(), col.ColliderSize,
							phy.physicsMaterial, col.ColliderPosition, col.ColliderOrientation);
					col.initialize = false;
					//col.collider->SetType(PhysicsCollider::CollisionType::Collider);
				}
			}
			else if (col.colliderType == col.ColliderType::Sphere)
			{
				if (phy.bodyType == phy.BodyType::Static && phy.staticBody && col.initialize)
				{
					col.ColliderPosition = tra.Position;
					col.collider = std::make_shared<SphereCollider>
						(phy.staticBody.get(), col.ColliderRadius,
							phy.physicsMaterial, col.ColliderPosition, col.ColliderOrientation);
					col.initialize = false;
					//col.collider->SetType(PhysicsCollider::CollisionType::Collider);
				}

				else if (phy.bodyType == phy.BodyType::Dynamic && phy.dynamicBody && col.initialize)
				{
					col.ColliderPosition = tra.Position;
					col.collider = std::make_shared<SphereCollider>
						(phy.dynamicBody.get(), col.ColliderRadius,
							phy.physicsMaterial, col.ColliderPosition, col.ColliderOrientation);
					col.initialize = false;
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
	}

	void Scene::InitScene()
	{
		//if(!m_DefaultsInitialized)
			//InitDefaults();

		// Physx not working correctly
		//InitPhysicsObjects();

		auto view = m_Registry.view<PhysicsComponent, ColliderComponent, TransformComponent, MaterialComponent>();
		for (auto entity : view)
		{
			auto& mat = view.get<MaterialComponent>(entity);
			//mat.m_Material = std::make_shared<Material>();
		}

		Renderer::Init(m_Registry);
	}

	Entity Scene::CreateMeshEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		//entity.AddComponent<ModelComponent>().AddMesh(m_AssetManager->LoadDefaultQuad(), (uint32_t)entity);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<ScriptComponent>();
		//entity.AddComponent<MaterialComponent>().m_Material = std::make_shared<Material>();
		//entity.GetComponent<MaterialComponent>().m_Material->SetPBRStatus(true);
		//entity.GetComponent<MaterialComponent>().isPBR = true;
		//entity.AddComponent<ModelComponent>().AddMesh(m_AssetManager->LoadGLTFModel("Assets/gltf_models/DamagedHelmet/glTF/DamagedHelmet.gltf"), (uint32_t)entity);
		entity.AddComponent<ModelComponent>().AddMesh(m_AssetManager->LoadGLTFModel("Assets/gltf_models/FlightHelmet/glTF/FlightHelmet.gltf"), (uint32_t)entity);
		entity.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Models/substance_sphere/marble/albedo.png"), 0);
		entity.GetComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Models/substance_sphere/ao.png"), 1);
		entity.GetComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Models/substance_sphere/marble/metallic.png"), 2);
		entity.GetComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Models/substance_sphere/marble/normal.png"), 3);
		entity.GetComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Models/substance_sphere/marble/roughness.png"), 4);
		entity.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/Shaders/ModelShader.glsl"));
		entity.AddComponent<Physics2DComponent>();
		entity.AddComponent<BoxCollider2DComponent>();
		
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	Entity Scene::CreateCubeEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<ModelComponent>().AddMesh(m_AssetManager->LoadGLTFModel("Assets/Models/rounded_cube/rounded_cube.obj"), (uint32_t)entity);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<ScriptComponent>();
		//entity.AddComponent<MaterialComponent>().m_Material = std::make_shared<Material>();
		entity.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/Shaders/ModelShader.glsl"));
		//entity.AddComponent<MaterialComponent>().m_Material = std::make_shared<Material>();
		entity.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Textures/rocky.jpg"), 0);
		entity.AddComponent<Physics2DComponent>();
		entity.AddComponent<BoxCollider2DComponent>();

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	Entity Scene::CreatePlaneEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<ScriptComponent>();
		entity.AddComponent<ModelComponent>().AddMesh(m_AssetManager->LoadGLTFModel("Assets/models/board/board.obj"), (uint32_t)entity);
		entity.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Models/board/albedo.png"), 0);
		entity.GetComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Models/board/ao.png"), 1);
		entity.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/Shaders/ModelPBRShader.glsl"));
		//entity.AddComponent<MaterialComponent>().m_Material = std::make_shared<Material>();
		//entity.GetComponent<MaterialComponent>().isPBR = false;
		//entity.GetComponent<MaterialComponent>().m_Material->SetPBRStatus(false);
		//entity.AddComponent<Physics2DComponent>();
		entity.AddComponent<BoxCollider2DComponent>();

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
		TextureConfiguration config{};
		config.m_MinFilter = Config::MinMagFilters::LINEAR;
		config.m_MagFilter = Config::MinMagFilters::LINEAR;
		config.m_TextureLayout = Config::TextureLayout::ClampToEdge;
		config.Path = entity.GetComponent<SkyboxComponent>().path;
		config.m_NullData = false;
		config.m_Flipped = true;
		entity.GetComponent<SkyboxComponent>().m_Skybox = Texture2D::Create(config);
		entity.GetComponent<SkyboxComponent>().m_Model = m_AssetManager->LoadGLTFModel("Assets/gltf_models/Cube/glTF/Cube.gltf");
		entity.GetComponent<SkyboxComponent>().m_Model->SetSkybox(true);
			entity.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/Shaders/SkyboxShader.glsl"));

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	Entity Scene::CreateCameraEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<CameraComponent>();
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

	void Scene::EnableGravity(bool gravity)
	{
		if (gravity)
			m_PhysicsWorld->SetGravity({ 0.0f, -9.8f });
		else
			m_PhysicsWorld->SetGravity({ 0.0f, 0.0f });
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
		EnableGravity(m_SceneSetting.enableGravity);

		auto view = m_Registry.view<Physics2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Physics2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Position.x, transform.Position.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}

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
		m_IsRunning = false;;

		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;

		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnSimulationStart()
	{
		m_IsSimulating = true;

		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
		EnableGravity(m_SceneSetting.enableGravity);
		{
			auto view = m_Registry.view<Physics2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Physics2DComponent>();

				b2BodyDef bodyDef;
				bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
				bodyDef.position.Set(transform.Position.x, transform.Position.y);
				bodyDef.angle = transform.Rotation.z;

				b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
				body->SetFixedRotation(rb2d.FixedRotation);
				rb2d.RuntimeBody = body;

				if (entity.HasComponent<BoxCollider2DComponent>())
				{
					auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

					b2PolygonShape boxShape;
					//boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);
					boxShape.SetAsBox(bc2d.Size.x, bc2d.Size.y);

					b2FixtureDef fixtureDef;
					fixtureDef.shape = &boxShape;
					fixtureDef.density = bc2d.Density;
					fixtureDef.friction = bc2d.Friction;
					fixtureDef.restitution = bc2d.Restitution;
					fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
					body->CreateFixture(&fixtureDef);
				}
			}
		}

		ScriptEngine::OnRuntimeStart(this);
		// Instantiate all script entities
		{
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}
	}

	void Scene::OnSimulationStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;

		m_IsSimulating = false;
	}

	void Scene::OnUpdateSimulation(Timestep ts, const EditorCamera& camera)
	{
		// Physics
		EnableGravity(m_SceneSetting.enableGravity);
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			auto view = m_Registry.view<Physics2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Physics2DComponent>();

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Position.x = position.x;
				transform.Position.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
		}

	}

	void Scene::OnUpdateResize(uint32_t width, uint32_t height)
	{

	}

	void Scene::OnUpdatePhysics(Timestep ts)
	{


#if ALBEDO_PHYSX
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
				//Albedo_Core_INFO("Rotation: {} {} {} {}", q.x, q.y, q.z, q.w);
				Albedo_Core_INFO("Position: {} {} {}", p.x, p.y, p.z);
				tra.AddTranform(glm::vec3(p.x, p.y, p.z), glm::vec4(q.x, q.y, q.z, q.w));

				phy.BodyPosition = glm::vec3(p.x, p.y, p.z);
				phy.BodyOrientation = glm::quat(q.x, q.y, q.z, q.w);
			}
			else if (phy.bodyType == phy.BodyType::Static && phy.staticBody)
			{
				//auto p = phy.staticBody->GetRigidActor()->getGlobalPose().p;
				//tra.AddTranform(glm::vec3(p.x, p.y, p.z));
				//phy.BodyPosition = glm::vec3(p.x, p.y, p.z);
			}
			else
				Albedo_Core_ERROR("Invalid Body Type");
		}
#endif
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
			auto view = m_Registry.view<ShaderComponent, TransformComponent, ModelComponent, TextureComponent>();

			for (auto entity : view)
			{
				auto& model = view.get<ModelComponent>(entity);
				auto& shader = view.get<ShaderComponent>(entity);
				//if (model.m_Model->GetInitializationStatus() || shader.m_Shader->GetInitializationStatus())
				//	InitScene();
			}
			for (auto entity : view)
			{
				if (m_Registry.any_of<CameraComponent>(entity)) continue;

				// TODO: fix
				//Renderer::Setup(*mainCamera, view.get<ShaderComponent>(entity), view.get<TransformComponent>(entity), view.get<TextureComponent>(entity));
				//Renderer::Render(view.get<ModelComponent>(entity), view.get<ModelComponent>(entity).m_Mesh->GetRendererConfig());
			}
		}
	}

	void Scene::OnUpdateEditor(EditorCamera& camera, Timestep ts)
	{
		auto view = m_Registry.view<ShaderComponent, TransformComponent, ModelComponent, 
			TextureComponent, MaterialComponent, ScriptComponent>();

		Camera* mainCamera = nullptr;

		if (m_IsSimulating)
		{
			// changing to Scene Camera
			//auto cameraView = m_Registry.view<TransformComponent, CameraComponent>();
			//for (auto entity : cameraView)
			//{
			//	auto [transform, newCamera] = cameraView.get<TransformComponent, CameraComponent>(entity);
			//
			//	if (newCamera.Primary)
			//	{
			//		mainCamera = &newCamera.Camera;
			//		((SceneCamera*)mainCamera)->SetPosition(transform.Position);
			//		camera =  *(EditorCamera*)mainCamera;
			//		break;
			//	}
			//}

			//OnUpdateSimulation(ts, camera);
			//OnUpdatePhysics(ts);

			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				//ScriptEngine::OnUpdateEntity(entity, ts);
				//Ref<ScriptInstance> instance = ScriptEngine::GetEntityScriptInstance(entity);
			}
		}

		for (auto& entity : view)
		{
			auto& tex = view.get<TextureComponent>(entity);
			//if (!view.get<MaterialComponent>(entity).isPBR) continue;
			auto& textures = view.get<TextureComponent>(entity).m_Textures;
			if (!textures[TextureComponent::TextureType::Albedo].get())
				tex.AddTexture(m_AssetManager->LoadTexture("Assets/Textures/DarkGrey.jpg"), (int)TextureComponent::TextureType::Albedo);

			if (!textures[TextureComponent::TextureType::AmbientOcclusion].get())
				tex.AddTexture(m_AssetManager->LoadTexture("Assets/Textures/DarkGrey.jpg"), (int)TextureComponent::TextureType::AmbientOcclusion);

			if (!textures[TextureComponent::TextureType::Metallic].get())
				tex.AddTexture(m_AssetManager->LoadTexture("Assets/Textures/DarkGrey.jpg"), (int)TextureComponent::TextureType::Metallic);

			if (!textures[TextureComponent::TextureType::Normal].get())
				tex.AddTexture(m_AssetManager->LoadTexture("Assets/Textures/DarkGrey.jpg"), (int)TextureComponent::TextureType::Normal);

			if (!textures[TextureComponent::TextureType::Roughness].get())
				tex.AddTexture(m_AssetManager->LoadTexture("Assets/Textures/DarkGrey.jpg"), (int)TextureComponent::TextureType::Roughness);
		}

		// Checking for re initialization of meshes
		auto meshView = m_Registry.view<ModelComponent, ShaderComponent>();
		for (auto& entity : meshView)
		{
			auto& mesh = meshView.get<ModelComponent>(entity);
			auto& shader = meshView.get<ShaderComponent>(entity);
			//if (mesh.m_Mesh->GetInitializationStatus() || shader.m_Shader->GetInitializationStatus())
			//	Renderer::Init(m_Registry);
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


		for (auto& entity : view)
		{

#if ALBEDO_PHYSX
			auto& pos = view.get<TransformComponent>(entity).Position;
			auto& rot = view.get<TransformComponent>(entity).Rotation;
			view.get<PhysicsComponent>(entity).BodyPosition = pos;
			view.get<PhysicsComponent>(entity).BodyOrientation = glm::quat(rot);
			view.get<ColliderComponent>(entity).ColliderPosition = pos;
#endif
		}

		//
		// Shadow map render pass
		//
		//Renderer::PreRenderPass(m_DepthShader, m_ShadowMap, m_Registry, lightDirection, tex);

		m_Framebuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto& entity : view)
		{
#if ALBEDO_PHYSX
			auto& pos = view.get<TransformComponent>(entity).Position;
			auto& rot = view.get<TransformComponent>(entity).Rotation;
			view.get<PhysicsComponent>(entity).BodyPosition = pos;
			view.get<PhysicsComponent>(entity).BodyOrientation = glm::quat(rot);
			view.get<ColliderComponent>(entity).ColliderPosition = pos;
#endif
		}

		auto skybox_view = m_Registry.view<ShaderComponent, SkyboxComponent, TransformComponent>();
		for (auto& entity : skybox_view)
		{
			glDepthMask(GL_FALSE);
			Renderer::SetupSkybox(camera, skybox_view.get<SkyboxComponent>(entity), skybox_view.get<ShaderComponent>(entity),
				skybox_view.get<TransformComponent>(entity).GetTransform());
			Renderer::Render(skybox_view.get<SkyboxComponent>(entity).m_Model, skybox_view.get<ShaderComponent>(entity).m_Shader);
			glDepthMask(GL_TRUE);
		}

		auto temp_view = m_Registry.view<ShaderComponent, ModelComponent, TransformComponent>();
		for (auto& entity : temp_view)
		{
			//Renderer::Setup();

			//if (view.get<MaterialComponent>(entity).m_Material->IsPBR())
			{
				//Renderer::SetupPBR(camera, (view.get<ShaderComponent>(entity)), view.get<TransformComponent>(entity),
				//	view.get<TextureComponent>(entity), view.get<MaterialComponent>(entity), lights);
			}
			//else if (!view.get<MaterialComponent>(entity).m_Material->IsPBR())
			{
				//Renderer::Setup(camera, view.get<ShaderComponent>(entity), view.get<TransformComponent>(entity),
				//	view.get<TextureComponent>(entity), view.get<MaterialComponent>(entity), lights);
				//Renderer::SetupPlane(camera, (view.get<ShaderComponent>(entity)), view.get<TransformComponent>(entity),
				//	view.get<TextureComponent>(entity), view.get<MaterialComponent>(entity), lights, m_ShadowMap);
			}
			Renderer::Setup(camera, view.get<ShaderComponent>(entity).m_Shader, view.get<TransformComponent>(entity).Transform);
			Renderer::Render(view.get<ModelComponent>(entity).m_Model, view.get<ShaderComponent>(entity).m_Shader);
		}

		// end
		return;
		auto phyView = m_Registry.view<TransformComponent, BoxCollider2DComponent, Physics2DComponent>();

		m_Transform = glm::mat4(1.0);
		for (auto& entity : phyView)
		{
			auto& tra = phyView.get<TransformComponent>(entity);
			auto& bc2d = phyView.get<BoxCollider2DComponent>(entity);
			m_Transform = glm::translate(glm::mat4(1.0f), tra.Position) * glm::scale(glm::mat4(1.0f), glm::vec3(bc2d.Size.x * 2, bc2d.Size.y * 2, 1.0));
			//m_Transform = glm::scale(glm::mat4(1.0f), glm::vec3(bc2d.Size.x * 2, bc2d.Size.y * 2, 0.0));
			
			if(m_SceneSetting.ShowCollider)
			{
				Renderer::Setup(camera, m_ColliderShader, m_Transform);
				Renderer::RenderOverlay(m_Collider);
			}
		}

		m_SkyboxShader->Bind();
		m_SkyboxShader->SetUniformMat4("projection", camera.GetProjection());
		m_SkyboxShader->SetUniformMat4("view", camera.GetViewMatrix());
		m_SkyboxShader->SetUniformInt1("environmentMap", 0);
		m_SkyboxShader->SetUniformFloat("darkness", m_SceneSetting.backgroundLight);
		
		bool skyboxExists = false;
		auto skyboxView = m_Registry.view<SkyboxComponent>();
		for (auto& e : skyboxView)
		{
			auto& skybox = skyboxView.get<SkyboxComponent>(e);
			if(skybox.m_Skybox)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.m_Skybox->GetTextureID());
				skyboxExists = true;
			}
		}
		if (!skyboxExists)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTemp->GetTextureID());
		}
				
		glDepthMask(GL_FALSE);
		//m_Skybox->GetMeshBufferData().m_VertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//m_Skybox->GetMeshBufferData().m_VertexArray->UnBind();
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

	Entity Scene::DuplicateEntity(Entity entity)
	{
		// Copy name because we're going to modify component data structure
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
		return newEntity;
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
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
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
	void Scene::OnComponentAdded<ModelComponent>(Entity entity, ModelComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<PhysicsComponent>(Entity entity, PhysicsComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Physics2DComponent>(Entity entity, Physics2DComponent& component)
	{
	}

}