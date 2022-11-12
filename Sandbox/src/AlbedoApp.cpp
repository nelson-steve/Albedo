#include <Albedo.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Albedo/Renderer/Texture.h"
#include "Albedo/Core.h"
#include "Albedo/Cameras/OrthographicCameraController.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gl/GL.h>
#include <gl/GLU.h>

class ExampleLayer : public Albedo::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_CameraController(1280.0f / 720.0f, true)
	{
		m_VertexArray.reset(Albedo::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		float boxVertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		float meshVertices[4 * 5] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		float triangleVertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		float twoTrianglesVertices[4 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
		};

		float meshTrianglesVertices[4 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
			 0.0f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f
		};

		//float cubeVertices[8 * 7] = {
		//	-0.25f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		//	 0.25f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		//	-0.25f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		//	 0.25f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		//	-0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
		//	 0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
		//	-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
		//	 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f
		//};

		float cubeVertices[8 * 7] = {
			-0.5f,  0.5f, 0.0f, 1.0f, 0.2f, 0.0f, 1.0f,
			 0.5f,  0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			-0.5f, -0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.1f, 0.8f, 0.2f, 1.0f,
			-0.5f,  0.5f, 0.5f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f,  0.5f, 0.5f, 0.2f, 0.3f, 0.8f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.3f, 0.8f, 0.6f, 1.0f,
			 0.5f, -0.5f, 0.5f, 0.2f, 0.6f, 0.0f, 1.0f
		};
		//glGenVertexArrays(1, &vao);
		//glBindVertexArray(vao);

		//glGenBuffers(1, &vbo);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);

		Albedo::Ref<Albedo::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Albedo::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));

		Albedo::BufferLayout layout =
		{
			{Albedo::ShaderDataType::Float3, "a_Position"},
			{Albedo::ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		unsigned int cubeIndices[36] = {0, 1, 2, 2, 1, 3,
										1, 5, 3, 3, 5, 7,
										0, 4, 2, 2, 4, 6,
										0, 1, 4, 4, 2, 5,
										2, 3, 6, 6, 3, 7,
										4, 5, 6, 6, 5, 7};
		unsigned int cubeIndices1[6] = { 0, 1, 2, 2, 1, 3 };
		unsigned int indices[6] = { 0, 1, 2, 2, 1, 3 };
		unsigned int boxIndices[6] = { 0, 1, 2, 2, 3, 0 };
		unsigned int meshIndices[6] = { 0, 1, 2, 2, 3, 0 };

		Albedo::Ref<Albedo::IndexBuffer> indexBuffer;
		indexBuffer.reset(Albedo::IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		//glGenBuffers(1, &ibo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


		// VERTEX SHADER

		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 a_Position;\n"
			"layout (location = 1) in vec2 a_Color;\n"
			"uniform mat4 u_ProjectionView;\n"
			"uniform mat4 u_Transform;\n"
			"uniform vec4 u_Color;\n"
			"out vec3 v_Position;\n"
			"out vec2 v_Color;\n"
			"void main()\n"
			"{\n"
			"v_Position = a_Position;\n"
			//"v_TexCoord = a_TexCoord;\n"
			"v_Color = a_Color;\n"
			"gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);\n"
			"}\0";

		// FRAGMENT SHADER

		const char* fragmentShaderSource = "#version 330 core\n"
			"layout (location = 0) out vec4 Color;\n"
			"in vec3 v_Position;\n"
			"in vec4 v_Color;\n"
			//"in vec2 v_TexCoord;\n"
			//"uniform sampler2D u_Texture;\n"
			"void main()\n"
			"{\n"
			"Color = v_Color;\n"
			"}\0";
		//new Albedo::Shader(vertexShaderSource, fragmentShaderSource)
		//m_Shader.reset(Albedo::Shader::Create(vertexShaderSource, fragmentShaderSource));
		//m_Shader.reset(Albedo::Shader::Create("Assets/Texture.glsl"));
		//m_Shader = Albedo::Shader::Create("Assets/Texture.glsl");
		m_Shader = Albedo::Shader::Create("Assets/Texture2.glsl");
		//m_Shader = Albedo::Shader::Create("MeshShader", vertexShaderSource, fragmentShaderSource);
		//std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader)->Bind();
		//std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader)->UploadUniformInt1("u_Texture", 0);

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
		void reshape(int width, int height) {  // GLsizei for non-negative integer
			// Compute aspect ratio of the new window
			if (height == 0) height = 1;                // To prevent divide by 0
			GLfloat aspect = (GLfloat)width / (GLfloat)height;

			// Set the viewport to cover the new window
			glViewport(0, 0, width, height);

			// Set the aspect ratio of the clipping volume to match the viewport
			glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
			glLoadIdentity();             // Reset
			// Enable perspective projection with fovy, aspect, zNear and zFar
			//gluPerspective(45.0f, aspect, 0.1f, 100.0f);
		}

		void OnUpdate(Albedo::Timestep ts)
		{
			m_CameraController.OnUpdate(ts);

			
			Albedo::RenderCommand::ClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
			Albedo::RenderCommand::Clear();

			Albedo::Renderer::BeginScene(m_CameraController.GetCamera());

			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
			glm::vec3 pos(-0.5f, 0.0f, 0.0f);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

			Albedo::Renderer::Submit(std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader), m_VertexArray, transform, m_BigColor);

			Albedo::Renderer::EndScene();

			//Albedo::Renderer::BeginScene(m_CameraController.GetCamera());
			//
			//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.8f));
			//glm::vec3 pos(-0.5f, 0.0f, 0.0f);
			//glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			//
			//m_Texture2 = Albedo::Texture2D::Create("TextureSample5.png"); //Any PNG image
			//
			//Albedo::Renderer::Submit(std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader), //std::dynamic_pointer_cast<Albedo::OpenGLTexture2D>(m_Texture2), m_VertexArray, transform, m_BigColor);
			//
			//scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.9f));
			//pos = glm::vec3(-0.9f, -0.5f, 0.0f);
			//transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			//
			//m_Texture = Albedo::Texture2D::Create("TextureSample6.png"); //Any PNG image
			//
			//Albedo::Renderer::Submit(std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader), //std::dynamic_pointer_cast<Albedo::OpenGLTexture2D>(m_Texture), m_VertexArray, transform, m_BigColor);
			//
			//Albedo::Renderer::EndScene();


			//Albedo::Renderer::BeginScene(m_CameraController.GetCamera());
			//
			//scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
			//glm::vec3 pos1(0.5f, 0.0f, 0.0f);
			//transform = glm::translate(glm::mat4(1.0f), pos1) * scale;
			//
			//m_Texture2 = Albedo::Texture2D::Create("TextureSample5.png"); //Any PNG image
			//
			//Albedo::Renderer::Submit(std::dynamic_pointer_cast<Albedo::OpenGLShader>(m_Shader), //std::dynamic_pointer_cast<Albedo::OpenGLTexture2D>(m_Texture2), m_VertexArray, transform, m_BigColor);
			//
			//Albedo::Renderer::EndScene();

		}

		virtual void OnImGuiRender() override
		{

		}

		void OnEvent(Albedo::Event& e) override
		{
			m_CameraController.OnEvent(e);
			//Albedo_TRACE("{0}", event);
		}
private:
	Albedo::Ref<Albedo::VertexArray> m_VertexArray;
	Albedo::Ref<Albedo::Shader> m_Shader;

	Albedo::Ref<Albedo::Texture2D> m_Texture;
	Albedo::Ref<Albedo::Texture2D> m_Texture2;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec4 m_BigColor = { 0.2, 0.4, 0.8, 1.0f };
	glm::vec4 m_SmallColor = { 0.5, 0.5, 0.9, 1.0f };

	Albedo::OrthographicCameraController m_CameraController;

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