#include <Albedo.h>
#include <Albedo/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Albedo {
	class AlbedoEditor : public Application {
	public:
		AlbedoEditor()
			:Application("Albedo Editor")
		{
			//PushLayer(new ExampleLayer());
			PushLayer(new EditorLayer());
		}
		~AlbedoEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new AlbedoEditor();
	}
}