#include <Albedo.h>

class ExampleLayer : public Albedo::Layer
{
public:
	ExampleLayer()
		:Layer("Example") {

	}

	void OnUpdate()
	{
		// mAlbedo_INFO("ExampleLayer::Update()");
	}

	virtual void OnImGuiRender() override
	{

	}

	void OnEvent(Albedo::Event& event) override
	{
		//Albedo_TRACE("{0}", event);
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