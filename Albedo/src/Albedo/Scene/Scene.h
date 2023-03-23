#pragma once

#include "Albedo/Cameras/EditorCamera.h"
#include "Albedo/Core/Timestep.h"

#include "Albedo/Renderer/Material.h"

#include "entt.hpp"

namespace Albedo {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void InitScene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(EditorCamera& camera, Timestep ts);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

		std::vector<Material*>& GetMaterialsInstance() { return m_Materials; }
		glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;

		inline static std::vector<Material*> m_Materials;
		float x = 0.001;
	};

}