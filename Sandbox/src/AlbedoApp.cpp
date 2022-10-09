#include <Albedo.h>

class ExampleLayer : public Albedo::Layer
{
public:
	ExampleLayer()
		:Layer("Example") {

	}

	void OnUpdate()
	{
		Albedo_INFO("ExampleLayer::Update()");
	}

	void OnEvent(Albedo::Event& event) override
	{
		Albedo_TRACE("{0}", event);
	}

};

class Sandbox : public Albedo::Application {
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{
	}
};

Albedo::Application* Albedo::CreateApplication()
{
	return new Sandbox();
}