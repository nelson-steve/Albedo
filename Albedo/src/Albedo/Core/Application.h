#pragma once

#include "Core.h"
#include "Window.h"
#include "Albedo/Layers/Layer.h"
#include "Albedo/Layers/LayerStack.h"
#include "Albedo/Events/Event.h"
#include "Albedo/Events/ApplicationEvent.h"
#include "Albedo/ImGui/ImGuiLayer.h"

namespace Albedo {

	class Application
	{
	public:
		Application(const std::string& name = "Albedo App");
		virtual ~Application();

		void Run();
		void Close();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		
		inline static Application& Get() { return *s_Instance; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		static Application* s_Instance;

		float m_LastFrameTime = 0.0f;
	};

	Application* CreateApplication();

}