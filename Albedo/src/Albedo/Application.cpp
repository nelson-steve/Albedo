#include "AlbedoPreCompiledHeader.h"
#include "Application.h"

#include "Albedo/Events/ApplicationEvent.h"
#include "Log.h"

#include <GLFW/glfw3.h>

namespace Albedo {

	Application::Application() 
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}
	Application::~Application() 
	{

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
		while (true) {
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}

}