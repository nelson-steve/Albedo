#pragma once

#include "Core.h"
#include "Window.h"
#include "Layers/Layer.h"
#include "Layers/LayerStack.h"
#include "Events/Event.h"
#include "Albedo/Events/ApplicationEvent.h"
#include "Albedo/ImGui/ImGuiLayer.h"

#include "Albedo/Renderer/Renderer.h"
#include "Renderer/VertexArray.h"
#include "Albedo/Renderer/Shader.h"
#include "Renderer/OrthographicCamera.h"

namespace Albedo {

	class Albedo_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<Shader> m_Shader;
		//std::shared_ptr<Shader> m_Shader;

		OrthographicCamera m_Camera;

		static Application* s_Instance;
	};

	Application* CreateApplication();

}