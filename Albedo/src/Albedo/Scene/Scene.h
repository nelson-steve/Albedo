#pragma once

#include "Albedo/Cameras/EditorCamera.h"
#include "Albedo/Core/Timestep.h"
#include "Albedo/Renderer/Material.h"
#include "Albedo/Renderer/Framebuffer.h"
#include "Platform/OpenGL/ShadowMap.h"
#include "Albedo/TerrainGeneration/TerrainManager.h"

#include "entt.hpp"

class b2World;

namespace Albedo {

	//
	// this is a scene class. it handles everything we see on the screen.
	//

	class Entity;
	class PhysicsSolver;
	class PhysicsWorld;

	struct SceneSetting
	{
		bool ShowCollider = false;
		bool enableGravity = true;
		float backgroundLight = 1.0;
	};

	class Scene
	{
	public:
		Scene(); // this is a constructor as you may remember. since it's name is the same as the class

		void InitDefaults();
		void InitPhysicsObjects();
		void InitScene();
		void ReInitScene();

		Entity CreateMeshEntity(const std::string& name = std::string());
		Entity CreateCubeEntity(const std::string& name = "Cube");
		Entity CreatePlaneEntity(const std::string& name = "Plane");
		Entity CreateLightEntity(const std::string& name = "Light");
		Entity CreateSkyboxEntity(const std::string& name = "Skybox");
		Entity CreateEntity(const std::string& name);
		Entity CreateCameraEntity(const std::string& name);
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, const EditorCamera& camera);
		void OnUpdateEditor(EditorCamera& camera, Timestep ts);
		void OnUpdateResize(uint32_t width, uint32_t height);

		void OnUpdatePhysics(Timestep ts);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
		SceneSetting& GetSceneSetting() { return m_SceneSetting; }
		Entity FindEntityByName(std::string_view name);

		Entity DuplicateEntity(Entity entity);
		
		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused; }

		void SetPaused(bool paused) { m_IsPaused = paused; }

		void SetMainFramebuffer(Ref<Framebuffer> fbo) { m_Framebuffer = fbo; }

		void Step(int frames = 1);

		void EnableGravity(bool gravity);

		entt::registry& Reg() { return m_Registry; }
		glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

		//TODO: make this private
		Ref<PhysicsSolver> m_PhysicsSolver;
		Ref<Texture2D> m_DepthMap;
		Ref<Framebuffer> m_DepthMapFBO;
		Ref<ShadowMap> m_ShadowMap;
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		Ref<Framebuffer> m_Framebuffer;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;
		bool m_IsSimulating = false;
		SceneSetting m_SceneSetting;

		b2World* m_PhysicsWorld = nullptr;
		//Ref<PhysicsWorld> m_PhysicsWorld3D;

		bool m_DefaultsInitialized = false;
		bool fbo = true;
		Ref<Texture2D> skyboxTemp;
		Ref<Shader> m_SkyboxShader;
		Ref<Shader> m_DepthShader;

		Ref<Texture2D> AlbedoMap;
		Ref<Texture2D> NormalMap;
		Ref<Texture2D> MetallicMap;
		Ref<Texture2D> RoughnessMap;
		Ref<Texture2D> AOMap;

		unsigned int depthMapFBO;

		Ref<Shader> m_TerrainShader;
		Ref<Shader> m_CubeShader;
		Ref<Texture2D> tex;

		Ref<Shader> m_ColliderShader;
		glm::mat4 m_Transform = glm::mat4(1.0);
		//Shader paths
		std::string m_PBRShaderPath = "Assets/Shaders/ModelPBRShader.glsl";
		std::string m_NonPBRShaderPath = "Assets/Shaders/ShadowMappingShader.glsl";
		std::string m_SkyboxShaderPath;
		std::string m_ShaderPath;
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		//inline static Ref<Skybox> m_Skybox; //TODO: Add Skybox
	};

}