#include <AlbedoPreCompiledHeader.h>
#include "Application.h"
#include "Log.h"
#include "Input.h"
#include "Albedo/Renderer/Renderer.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Albedo {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		Albedo_PROFILE_FUNCTION();
		s_Instance = this;

		m_Window = Scope<Window>(Window::Create());
		//m_Window = Window::Create();
		m_Window->SetEventCallBack(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}
	Application::~Application() 
	{
		//Albedo_PROFILE_FUNCTION();
		//Renderer::Shutdown();
	}

	void Application::OnEvent(Event& e)
	{
		Albedo_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}

		//Albedo_TRACE("{0}", e);
	}

	void Application::Run()
	{
		Albedo_PROFILE_FUNCTION();
		//WindowResizeEvent e(1280, 720);
		//if(e.IsInCategory(EventCategoryApplication))
		//{
		//	Albedo_TRACE(e);
		//}
		//if (e.IsInCategory(EventCategoryInput))
		//{
		//	Albedo_Core_TRACE(e);
		//}
		while (m_Running) 
		{
			Albedo_PROFILE_FUNCTION();
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					Albedo_PROFILE_FUNCTION();
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}
				{
					Albedo_PROFILE_FUNCTION();
					m_ImGuiLayer->Begin();
					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		Albedo_PROFILE_FUNCTION();
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::PushLayer(Layer* layer)
	{
		Albedo_PROFILE_FUNCTION();
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		Albedo_PROFILE_FUNCTION();
		m_LayerStack.PushOverlay(layer);
	}
}