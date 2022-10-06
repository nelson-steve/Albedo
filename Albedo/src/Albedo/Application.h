#pragma once

#include "Albedo/Events/Event.h"

namespace Albedo {

	class Albedo_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}