#pragma once

#include "Albedo/Core/Core.h"
#include "Albedo/Core/Log.h"
#include "Albedo/Scene/Scene.h"
#include "Albedo/Scene/Entity.h"

namespace Albedo {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel();
		SceneHierarchyPanel(const Ref<Scene> scene);

		void SetContext(const Ref<Scene> scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponentsOfEntity(Entity entity);
		void DrawComponentsOfScene(Entity entity);
	private:
		float exposure = 2.2f;
		float roughness = 1.4f;
		glm::vec3 lightPos = glm::vec3(1.0);
		glm::vec3 color = glm::vec3(1.0);

		bool showCollider = true;
		bool isPBR = false;
		bool default = false;
		bool m_Albedo = false, m_AmbientOcclusion = false, 
			m_Metallic = false, m_Normal = false, m_Roughness = false;
		std::string m_CurrentShader = "Vertex";
		std::string m_CurrentTexture = "Albedo";
		std::string m_CurrentLight = "Point";
		std::string m_CurrentPhysicsType = "Dynamic";
		std::string m_CurrentColliderType = "Box";
		float offset = 0;
		Ref<Texture2D> m_MeshIcon;
		Ref<Texture2D> m_BlackTexture;
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}