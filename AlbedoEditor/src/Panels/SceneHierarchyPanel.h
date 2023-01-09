#pragma once

#include "Albedo/Core/Core.h"
#include "Albedo/Core/Log.h"
#include "Albedo/Scene/Scene.h"
#include "Albedo/Scene/Entity.h"

namespace Albedo {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}