#include <AlbedoPreCompiledHeader.h>
#include "Application.h"
#include "Log.h"
#include "Input.h"

#include <glad/glad.h>

namespace Albedo {

	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application() 
		:m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		//m_Window = Window::Create();
		m_Window->SetEventCallBack(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);


		m_VertexArray.reset(VertexArray::Create());
		
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		
		//glGenVertexArrays(1, &vao);
		//glBindVertexArray(vao);

		//glGenBuffers(1, &vbo);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout =
		{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		unsigned int indices[3] = { 0, 1, 2 };
		
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		//glGenBuffers(1, &ibo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


		// VERTEX SHADER

		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 a_Position;\n"
			"layout (location = 1) in vec4 a_Color;\n"
			"uniform mat4 u_ProjectionView;\n"
			"out vec3 v_Position;\n"
			"out vec4 v_Color;\n"
			"void main()\n"
			"{\n"
				"v_Position = a_Position;\n"
				"v_Color = a_Color;\n"
				"gl_Position = u_ProjectionView * vec4(a_Position, 1.0);\n"
			"}\0";

		// FRAGMENT SHADER

		const char* fragmentShaderSource = "#version 330 core\n"
			"layout (location = 0) out vec4 Color;\n"
			"in vec3 v_Position;\n"
			"in vec4 v_Color;\n"
			"void main()\n"
			"{\n"
				"Color = v_Color;\n"
			"}\0";

		m_Shader.reset(new Shader(vertexShaderSource, fragmentShaderSource));
			
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
	Application::~Application() 
	{
		//glDeleteShader(vertexShader);
		//glDeleteShader(fragmentShader);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}

		//Albedo_TRACE("{0}", e);
	}

	void Application::Run()
	{
		//WindowResizeEvent e(1280, 720);
		//if(e.IsInCategory(EventCategoryApplication))
		//{
		//	Albedo_TRACE(e);
		//}
		//if (e.IsInCategory(EventCategoryInput))
		//{
		//	Albedo_Core_TRACE(e);
		//}
		while (m_Running) {
			RenderCommand::ClearColor({0.2f, 0.2f, 0.2f, 0.2f});
			RenderCommand::Clear();

			m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });
			m_Camera.SetRotation(45.0f);

			Renderer::BeginScene(m_Camera);

			Renderer::Submit(m_Shader, m_VertexArray);

			Renderer::EndScene;

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}
}