#pragma once

#include "Albedo.h"
#include "Panels/SceneHierarchyPanel.h"

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

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		OrthographicCameraController m_CameraController;

		Ref<VertexArray> m_VertexArray;
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_Texture;
		Ref<Texture2D> m_Texture1;
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		bool m_PrimaryCamera = true;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		//struct ProfileResult
		//{
		//	const char* Name;
		//	float Time;
		//};

		//std::vector<ProfileResult> m_ProfileResults;

		glm::vec4 m_SquareColor = { 0.0f, 0.0f, 1.0f, 1.0f };

		int m_GizmoType = -1;

		SceneHierarchyPanel m_SceneHierarchyPanel;

	};
}