#pragma once

#include "Albedo/Cameras/EditorCamera.h"
#include "Albedo/Core/Timestep.h"
#include "Albedo/Renderer/Material.h"
#include "Albedo/Renderer/Mesh.h"
#include "Albedo/Physics/PhysicsCollider.h"
#include "Albedo/Renderer/Framebuffer.h"

#include "entt.hpp"

namespace Albedo {

	class Entity;
	class PhysicsSolver;

	class Scene
	{
	public:
		Scene();

		void InitScene();
		void ReInitScene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
		void OnUpdateEditor(EditorCamera& camera, Timestep ts);
		void OnUpdateResize(uint32_t width, uint32_t height);

		void OnUpdatePhysics(Timestep ts);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
		Entity FindEntityByName(std::string_view name);
		
		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused; }

		void SetPaused(bool paused) { m_IsPaused = paused; }

		void Step(int frames = 1);

		entt::registry& Reg() { return m_Registry; }
		glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

		//TODO: make this private
		Ref<PhysicsSolver> m_PhysicsSolver;
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;
		bool m_IsSimulating = false;

		Ref<Texture2D> skyboxTemp;

		Ref<Texture2D> hdrTexture;
		//Ref<Texture2D> envCubemap;
		//Ref<Texture2D> irradianceMap;
		//Ref<Texture2D> prefilterMap;
		//Ref<Texture2D> brdfLUTTexture;
		unsigned int envCubemap;
		unsigned int irradianceMap;
		unsigned int prefilterMap;
		unsigned int brdfLUTTexture;


		Ref<Texture2D> AlbedoMap;
		Ref<Texture2D> NormalMap;
		Ref<Texture2D> MetallicMap;
		Ref<Texture2D> RoughnessMap;
		Ref<Texture2D> AOMap;

		Ref<Shader> pbrShader;
		Ref<Shader>	equirectangularToCubemapShader;
		Ref<Shader>	irradianceShader;
		Ref<Shader>	prefilterShader;
		Ref<Shader>	brdfShader;
		Ref<Shader>	backgroundShader;

		Ref<Framebuffer> m_Framebuffer;
		Ref<Shader> m_SkyboxShader;
		Ref<Shader> m_IrradianceShader;
		Ref<Shader> m_PrefilterShader;
		Ref<Shader> m_brdfShader;

		//Ref<Shader> m_ShaderTemp;
		Ref<Shader> m_CubeShader;
		Ref<Mesh> m_Cube;
		Ref<Texture2D> tex;

		Ref<Mesh> m_Skybox;
		Ref<Mesh> m_Collider;
		Ref<Mesh> m_Quad;
		Ref<Shader> m_Shader;
		glm::mat4 m_Transform = glm::mat4(1.0);
		std::vector<Ref<PhysicsCollider>> m_ColliderList;
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		//inline static Ref<Skybox> m_Skybox; //TODO: Add Skybox
	};

}