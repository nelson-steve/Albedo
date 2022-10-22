#include <Albedo.h>

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

		//glGenVertexArrays(1, &vao);
		//glBindVertexArray(vao);

		//glGenBuffers(1, &vbo);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);

		std::shared_ptr<Albedo::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Albedo::VertexBuffer::Create(vertices, sizeof(vertices)));

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

		unsigned int indices[3] = { 0, 1, 2 };

		std::shared_ptr<Albedo::IndexBuffer> indexBuffer;
		indexBuffer.reset(Albedo::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

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

		m_Shader.reset(new Albedo::Shader(vertexShaderSource, fragmentShaderSource));

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

		void OnUpdate()
		{
			Albedo::RenderCommand::ClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
			Albedo::RenderCommand::Clear();

			m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });
			m_Camera.SetRotation(45.0f);

			Albedo::Renderer::BeginScene(m_Camera);

			Albedo::Renderer::Submit(m_Shader, m_VertexArray);

			Albedo::Renderer::EndScene;
		}

		virtual void OnImGuiRender() override
		{

		}

		void OnEvent(Albedo::Event& event) override
		{
			//Albedo_TRACE("{0}", event);
		}
private:
	std::shared_ptr<Albedo::VertexArray> m_VertexArray;
	std::shared_ptr<Albedo::Shader> m_Shader;

	Albedo::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;

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