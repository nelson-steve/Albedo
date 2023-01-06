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
		#ifdef BATCH
		Albedo::BatchRenderer2D::BeginScene(m_CameraController.GetCamera());
		//Albedo::BatchRenderer2D::DrawQuad({ 5.0f, 5.0f, -0.1f }, { 2.5f, 2.5f }, m_Texture1);
		float value = 10.0f;
		for (float y = -value; y < value; y += 0.5f)
		{
			for (float x = -value; x < value; x += 0.5f)
			{
				glm::vec4 color = { (x + value) / 10.0f, 0.4f, (y + value) / 10.0f, 0.7f };
				Albedo::BatchRenderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		//Albedo::BatchRenderer2D::DrawQuad({ 0.0f, 0.0f, -0.2f }, { 2.5f, 2.5f }, m_Texture);
		Albedo::BatchRenderer2D::EndScene();
		#endif

		#ifndef BATCH
		Albedo::Renderer2D::BeginScene(m_CameraController.GetCamera());
		float value = 10.0f;
		for (float y = -value; y < value; y += 0.5f)
		{
			for (float x = -value; x < value; x += 0.5f)
			{
				glm::vec4 color = { (x + value) / 10.0f, 0.4f, (y + value) / 10.0f, 0.7f };
				Albedo::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		//Albedo::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
		Albedo::Renderer2D::EndScene();
		#endif // !1



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
	static bool dockingEnabled = false;
	if (dockingEnabled)
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit")) Albedo::Application::Get().Close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Begin("Settings");
		/*
		auto stats = Albedo::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		*/
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

		uint32_t textureID = m_Texture->GetTextureID();
		ImGui::Image((void*)textureID, ImVec2{ 256.0f, 256.0f });
		ImGui::End();

		ImGui::End();
	}
	//ImGui::Begin("Settings");
	//ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	//for (auto& result : m_ProfileResults)
	//{
	//	char label[50];
	//	strcpy(label, "%.3fms ");
	//	strcat(label, result.Name);
	//	ImGui::Text(label, result.Time);
	//}
	//m_ProfileResults.clear();

	//ImGui::End();
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