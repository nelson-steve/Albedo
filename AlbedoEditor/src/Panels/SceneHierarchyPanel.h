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
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		bool default = false;
		bool m_Albedo = false, m_AmbientOcclusion = false, 
			m_Metallic = false, m_Normal = false, m_Roughness = false;
		std::string m_CurrentShader = "Vertex";
		std::string m_CurrentTexture = "Albedo";
		std::string m_CurrentPhysicsType = "Static";
		std::string m_CurrentColliderType = "Sphere";
		bool m_PhysicsEnabled;
		float offset = 0;
		Ref<Texture2D> m_MeshIcon;
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}