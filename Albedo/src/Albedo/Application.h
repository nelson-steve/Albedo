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
		//std::shared_ptr<Shader> m_Shader;


		static Application* s_Instance;

		unsigned int vbo, vao, ibo;
		unsigned int vertexShader;
		unsigned int fragmentShader;
		unsigned int shaderProgram;
	};

	Application* CreateApplication();

}