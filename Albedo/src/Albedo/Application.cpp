#include "AlbedoPreCompiledHeader.h"
#include "Application.h"

#include "Log.h"

#include <GLFW/glfw3.h>

namespace Albedo {

	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application() 
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(BIND_EVENT_FN(OnEvent));
	}
	Application::~Application() 
	{

	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		Albedo_TRACE("{0}", e);
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
		while (m_Running) {
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}