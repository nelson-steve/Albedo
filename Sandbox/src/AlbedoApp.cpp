#include <Albedo.h>
#include <Albedo/Core/EntryPoint.h>

#include "AlbedoApp2D.h"

class Sandbox : public Albedo::Application {
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new AlbedoApp2D());
	}
	~Sandbox()
	{
	}
};

Albedo::Application* Albedo::CreateApplication()
{
	return new Sandbox();
}