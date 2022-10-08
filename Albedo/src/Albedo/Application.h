#pragma once

#include "Core.h"
#include "Window.h"
#include "Events/Event.h"
#include "Albedo/Events/ApplicationEvent.h"

namespace Albedo {

	class Albedo_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	Application* CreateApplication();
}