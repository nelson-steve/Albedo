#include <Albedo.h>

#include "imgui/imgui.h"



class ExampleLayer : public Albedo::Layer
{
public:
	ExampleLayer()
		:Layer("Example") {

	}

	void OnUpdate()
	{
		if (Albedo::Input::IsKeyPressed(Albedo_KEY_TAB))
		{
			Albedo_TRACE("Tab key is pressed");
		}
		// mAlbedo_INFO("ExampleLayer::Update()");
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
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