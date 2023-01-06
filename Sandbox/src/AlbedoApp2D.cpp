#include "AlbedoApp2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"
#include "Albedo/Core/Core.h"
#include "Albedo/Cameras/OrthographicCameraController.h"

AlbedoApp2D::AlbedoApp2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{

}

void AlbedoApp2D::OnAttach()
{
	Albedo_PROFILE_FUNCTION();
	m_Texture = Albedo::Texture2D::Create("TextureSample5.png");
	m_Texture1 = Albedo::Texture2D::Create("TextureSample6.png");
}

void AlbedoApp2D::OnDetach()
{
	Albedo_PROFILE_FUNCTION();
}

void AlbedoApp2D::OnUpdate(Albedo::Timestep ts)
{
	Albedo_PROFILE_FUNCTION("Sandbox2D::OnUpdate");

	{
		Albedo_PROFILE_FUNCTION("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);

	}
	{
		Albedo_PROFILE_FUNCTION("Render Prep");
		Albedo::RenderCommand::ClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
		Albedo::RenderCommand::Clear();
	}

	{
		Albedo_PROFILE_FUNCTION("Render Draw");

		//Albedo::_Renderer2D::BeginScene(m_CameraController.GetCamera());
		//Albedo::_Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
		//Albedo::_Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_Texture);
		//Albedo::_Renderer2D::EndScene();

		Albedo::BatchRenderer2D::BeginScene(m_CameraController.GetCamera());
		Albedo::BatchRenderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
		Albedo::BatchRenderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 2.5f, 2.5f }, m_Texture1);
		Albedo::BatchRenderer2D::EndScene();
		//Albedo::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//Albedo::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
		//Albedo::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 2.5f, 2.5f }, m_Texture1);
		//Albedo::Renderer2D::DrawQuad({ 0.5f, 0.3f }, { 0.5f, 0.2f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		//Albedo::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
		//Albedo::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.2f }, { 5.0f, 5.0f }, m_Texture1);
		//Albedo::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture);
		//Albedo::Renderer2D::DrawQuad({ 0.8f, 0.5f }, { 0.2f, 1.0f }, m_SquareColor);
		//Albedo::Renderer2D::DrawRotatedQuad({ 0.5f, 0.3f }, { 0.5f, 0.2f }, glm::radians(40.0f), { 0.8f, 0.0f, 0.0f, 1.0f });
		//Albedo::Renderer2D::EndScene();
	}
}

void AlbedoApp2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	//for (auto& result : m_ProfileResults)
	//{
	//	char label[50];
	//	strcpy(label, "%.3fms ");
	//	strcat(label, result.Name);
	//	ImGui::Text(label, result.Time);
	//}
	//m_ProfileResults.clear();

	ImGui::End();
}

void AlbedoApp2D::OnEvent(Albedo::Event& e)
{
	Albedo_PROFILE_FUNCTION();
	m_CameraController.OnEvent(e);	
}

/*
#include <chrono>

template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func)
		: m_Name(name), m_Func(func), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;
		m_Func({ m_Name, duration });
	}
private:
	const char* m_Name;
	Fn m_Func;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })
*/