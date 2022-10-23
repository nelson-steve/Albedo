#include <Albedo.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Albedo/Renderer/Texture.h"
#include "Albedo/Core.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Albedo::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Albedo::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		float boxVertices[4 * 5] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		//glGenVertexArrays(1, &vao);
		//glBindVertexArray(vao);

		//glGenBuffers(1, &vbo);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);

		Albedo::Ref<Albedo::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Albedo::VertexBuffer::Create(boxVertices, sizeof(boxVertices)));

		Albedo::BufferLayout layout =
		{
			{Albedo::ShaderDataType::Float3, "a_Position"},
			{Albedo::ShaderDataType::Float2, "a_TexCoord"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		unsigned int indices[3] = { 0, 1, 2 };
		unsigned int boxIndices[6] = { 0, 1, 2, 2, 3, 0 };

		Albedo::Ref<Albedo::IndexBuffer> indexBuffer;
		indexBuffer.reset(Albedo::IndexBuffer::Create(boxIndices, sizeof(boxIndices) / sizeof(unsigned int)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		//glGenBuffers(1, &ibo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


		// VERTEX SHADER

		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 a_Position;\n"
			"layout (location = 1) in vec2 a_TexCoord;\n"
			"uniform mat4 u_ProjectionView;\n"
			"uniform mat4 u_Transform;\n"
			"uniform vec4 u_Color;\n"
			//"out vec3 v_Position;\n"
			"out vec2 v_TexCoord;\n"
			"void main()\n"
			"{\n"
			//"v_Position = a_Position;\n"
			"v_TexCoord = a_TexCoord;\n"
			"gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);\n"
			"}\0";

		// FRAGMENT SHADER

		const char* fragmentShaderSource = "#version 330 core\n"
			"layout (location = 0) out vec4 Color;\n"
			//"in vec3 v_Position;\n"
			//"in vec4 v_Color;\n"
			"in vec2 v_TexCoord;\n"
			"uniform sampler2D u_Texture;\n"
			"void main()\n"
			"{\n"
			"Color = texture(u_Texture, v_TexCoord);\n"
			"}\0";
		//new Albedo::Shader(vertexShaderSource, fragmentShaderSource)
		m_Shader.reset(Albedo::Shader::Create(vertexShaderSource, fragmentShaderSource));

		m_Texture = Albedo::Texture2D::Create("Sample.png"); //Any PNG image

		std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader)->Bind();
		std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader)->UploadUniformInt1("u_Texture", 0);

		//vertexShader = glCreateShader(GL_VERTEX_SHADER);
		//
		//glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		//glCompileShader(vertexShader);

		//int  success;
		//char infoLog[512];
		//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		//
		//if (!success)
		//{
		//	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		//	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		//}

		//fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		//
		//glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		//glCompileShader(fragmentShader);
		//
		//shaderProgram = glCreateProgram();
		//
		//glAttachShader(shaderProgram, vertexShader);
		//glAttachShader(shaderProgram, fragmentShader);
		//glLinkProgram(shaderProgram);
		//
		//glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		//
		//glUseProgram(shaderProgram);
	}

		void OnUpdate(Albedo::Timestep ts)
		{
			if (Albedo::Input::IsKeyPressed(Albedo_KEY_TAB))
				Albedo_TRACE("Tab key is pressed (poll)!");
			if (Albedo::Input::IsKeyPressed(Albedo_KEY_LEFT))
				m_CameraPosition.x -= m_CameraMoveSpeed * ts;
			else if (Albedo::Input::IsKeyPressed(Albedo_KEY_RIGHT))
				m_CameraPosition.x += m_CameraMoveSpeed * ts;

			if (Albedo::Input::IsKeyPressed(Albedo_KEY_UP))
				m_CameraPosition.y += m_CameraMoveSpeed * ts;
			else if (Albedo::Input::IsKeyPressed(Albedo_KEY_DOWN))
				m_CameraPosition.y -= m_CameraMoveSpeed * ts;

			if (Albedo::Input::IsKeyPressed(Albedo_KEY_A))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			if (Albedo::Input::IsKeyPressed(Albedo_KEY_D))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			if (Albedo::Input::IsKeyPressed(Albedo_KEY_R)) //Camera Reset
			{
				m_CameraRotation = 0.0f;
				m_CameraPosition = glm::vec3(0.0f);
			}
				

			Albedo::RenderCommand::ClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
			Albedo::RenderCommand::Clear();

			m_Camera.SetPosition(m_CameraPosition);
			m_Camera.SetRotation(m_CameraRotation);

			Albedo::Renderer::BeginScene(m_Camera);

			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
			glm::vec3 pos(-0.5f, 0.0f, 0.0f);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

			Albedo::Renderer::Submit(std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader), std::dynamic_pointer_cast<Albedo::OpenGLTexture2D>(m_Texture), m_VertexArray, transform, m_BigColor);

			Albedo::Renderer::EndScene();


			Albedo::Renderer::BeginScene(m_Camera);
			
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
			glm::vec3 pos1(0.5f, 0.0f, 0.0f);
			transform = glm::translate(glm::mat4(1.0f), pos1) * scale;

			Albedo::Renderer::Submit(std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader), m_VertexArray, transform, m_SmallColor);

			Albedo::Renderer::EndScene();

		}

		virtual void OnImGuiRender() override
		{

		}

		void OnEvent(Albedo::Event& event) override
		{
			//Albedo_TRACE("{0}", event);
		}
private:
	Albedo::Ref<Albedo::VertexArray> m_VertexArray;
	Albedo::Ref<Albedo::Shader> m_Shader;

	Albedo::Ref<Albedo::Texture2D> m_Texture;

	Albedo::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec4 m_BigColor = { 0.2, 0.4, 0.8, 1.0f };
	glm::vec4 m_SmallColor = { 0.5, 0.5, 0.9, 1.0f };

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