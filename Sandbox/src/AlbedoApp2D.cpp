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
	m_VertexArray = (Albedo::VertexArray::Create());

	float squareVertices[4 * 7] = {
		-0.5f,  0.5f, 0.0f, 1.0f, 0.2f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.1f, 0.8f, 0.2f, 1.0f
	};

	Albedo::Ref<Albedo::VertexBuffer> vertexBuffer;
	vertexBuffer.reset(Albedo::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

	Albedo::BufferLayout layout =
	{
		{Albedo::ShaderDataType::Float3, "a_Position"},
		{Albedo::ShaderDataType::Float4, "a_Color"}
	};

	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	unsigned int cubeIndices[6] = { 0, 1, 2, 2, 1, 3 };

	Albedo::Ref<Albedo::IndexBuffer> indexBuffer;
	indexBuffer.reset(Albedo::IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int)));
	m_VertexArray->SetIndexBuffer(indexBuffer);
m_Shader = Albedo::Shader::Create("Assets/TextureSquare.glsl");
}

void AlbedoApp2D::OnDetach()
{

}

void AlbedoApp2D::OnUpdate(Albedo::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Albedo::RenderCommand::ClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
	Albedo::RenderCommand::Clear();

	Albedo::Renderer::BeginScene(m_CameraController.GetCamera());

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	glm::vec3 pos(-0.5f, 0.0f, 0.0f);
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

	Albedo::Renderer::Submit(std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader), m_VertexArray, transform, m_SquareColor);

	Albedo::Renderer::EndScene();
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