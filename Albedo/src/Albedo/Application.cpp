#include "Application.h"

#include "Albedo/Events/ApplicationEvent.h"
#include "Log.h"

namespace Albedo {

	Application::Application() 
	{

	}
	Application::~Application() 
	{

	}
	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if(e.IsInCategory(EventCategoryApplication))
		{
			Albedo_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			Albedo_Core_TRACE(e);
		}
		while (true);
	}

}