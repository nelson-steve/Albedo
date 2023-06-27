#pragma once

#include "Albedo/Cameras/EditorCamera.h"
#include "Albedo/Core/Timestep.h"
#include "Albedo/Renderer/Material.h"
#include "Albedo/Renderer/Mesh.h"
#include "Albedo/Physics/PhysicsCollider.h"
#include "Albedo/Renderer/Framebuffer.h"
#include "Platform/OpenGL/ShadowMap.h"
#include "Albedo/TerrainGeneration/TerrainChunk.h"

#include "entt.hpp"

namespace Albedo {

	//
	// this is a scene class. it handles everything we see on the screen.
	//

	class Entity;
	class PhysicsSolver;

	class Scene
	{
	public:
		Scene(); // this is a constructor as you may remember. since it's name is the same as the class

		void InitDefaults();
		void InitPhysicsObjects();
		void InitScene();
		void ReInitScene();

		Entity CreateMeshEntity(const std::string& name = std::string());
		Entity CreateLightEntity(const std::string& name = std::string());
		Entity CreateSkyboxEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
		void OnUpdateEditor(const EditorCamera& camera, Timestep ts);
		void OnUpdateResize(uint32_t width, uint32_t height);

		void OnUpdatePhysics(Timestep ts);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
		Entity FindEntityByName(std::string_view name);
		
		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused; }

		void SetPaused(bool paused) { m_IsPaused = paused; }

		void SetMainFramebuffer(Ref<Framebuffer> fbo) { m_Framebuffer = fbo; }

		void Step(int frames = 1);

		entt::registry& Reg() { return m_Registry; }
		glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

		//TODO: make this private
		Ref<PhysicsSolver> m_PhysicsSolver;
		Ref<Texture2D> m_DepthMap;
		Ref<Framebuffer> m_DepthMapFBO;
		Ref<ShadowMap> m_ShadowMap;
		Ref<TerrainChunk> m_TerrainChunk;
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
		Ref<Mesh> m_Cube;
		Ref<Texture2D> tex;

		Ref<Mesh> m_Skybox;
		Ref<Mesh> m_Collider;
		Ref<Mesh> m_Quad;
		Ref<Shader> m_Shader;
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