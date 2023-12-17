#include <AlbedoPreCompiledHeader.h>

#include "Application.h"
#include "Albedo/Renderer/Renderer.h"
#include "Albedo/Utils/AssetSystem.h"
#include "Albedo/Scripting/ScriptEngine.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Albedo {

	extern std::unique_ptr<Albedo::AssetSystem> m_AssetManager = std::make_unique<Albedo::AssetSystem>();

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
	{
		s_Instance = this;

		m_Window = Scope<Window>(Window::Create(WindowProps(name)));
		//m_Window = Window::Create();
		m_Window->SetEventCallBack(BIND_EVENT_FN(Application::OnEvent));

		RenderCommand::Init();
		ScriptEngine::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}
	Application::~Application() 
	{
		//Renderer::Shutdown();
	}

	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		m_MainThreadQueue.emplace_back(function);
	}


	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		 
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
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
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			ExecuteMainThreadQueue();

			if (!m_Minimized)
			{
				{
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}
				{
					m_ImGuiLayer->Begin();
					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		for (auto& func : m_MainThreadQueue)
			func();

		m_MainThreadQueue.clear();
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}
}