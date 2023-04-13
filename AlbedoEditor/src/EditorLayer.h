#pragma once

#include "Albedo.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Albedo/Cameras/EditorCamera.h"
#include <Albedo/Utils/AssetSystem.h>

namespace Albedo {
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
		void OnScenePlay();
		void OnSceneStop();

		// UI Panels
		void UI_Toolbar();
		void OpenScene(const std::filesystem::path& path);
	private:
		OrthographicCameraController m_CameraController;
		Ref<Scene>					 m_ActiveScene;
		//Ref<PhysicsWorld>			 m_PhysicsWorld;
		Ref<Framebuffer>			 m_Framebuffer;
		Ref<Texture2D>				 m_IconPlay;
		Ref<Texture2D>				 m_IconStop;
		EditorCamera				 m_EditorCamera;
		Entity						 m_HoveredEntity;
		glm::vec2					 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2					 m_ViewportBounds[2];
		bool						 m_PrimaryCamera = true;
		bool						 m_ViewportFocused = false;
		bool						 m_ViewportHovered = false;
		int							 m_GizmoType = -1;
		SceneHierarchyPanel			 m_SceneHierarchyPanel;
		ContentBrowserPanel			 m_ContentBrowserPanel;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;

		friend class SceneHierarchyPanel;

	};
}