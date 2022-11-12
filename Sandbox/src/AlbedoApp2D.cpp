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

}

void AlbedoApp2D::OnDetach()
{

}

void AlbedoApp2D::OnUpdate(Albedo::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Albedo::RenderCommand::ClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
	Albedo::RenderCommand::Clear();

	Albedo::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Albedo::Renderer2D::DrawQuad({0.0f, 0.0f}, { 1.0f, 1.0f}, m_SquareColor);
	Albedo::Renderer2D::EndScene();
}

void AlbedoApp2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void AlbedoApp2D::OnEvent(Albedo::Event& e)
{
	m_CameraController.OnEvent(e);	
}