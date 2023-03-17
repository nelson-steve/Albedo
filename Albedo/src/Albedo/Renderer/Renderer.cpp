#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

namespace Albedo {
	
	uint32_t cubeIndices[36] = {
		0, 2, 1, 0, 3, 2, // FRONT
		4, 1, 5, 4, 0, 1, // LEFT
		3, 6, 2, 3, 7, 6, // RIGHT
		5, 2, 1, 5, 6, 2, // TOP
		4, 3, 0, 4, 7, 3, // BOTTOM
		4, 6, 5, 4, 7, 6  // BACK
	};

	struct line {
		float Point1;
		float Point2;
	};

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::InitMaterials(const std::vector<Material*>& materials)
	{
		float skyboxVertices[] =
		{
			-1.0f,  1.0f, -1.0f,  
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		float cubeVertices_[] = {
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};

		float cubeVertices[] = {
			// positions  // texture Coords    // normals
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
		};

		float lineVertices[2 * 3] = {
			-0.5f, 0.0f, 0.0f,
			 0.5f, 0.0f, 0.0f
		};
#if 0
		float vertices[] = {
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};

		float cubeVertices_[] = 
		{
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f,
			 1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f
		};

		float cubeVertices__[8 * 3] = {
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f
		};
#endif

		for (Material* material : materials)
		{
			switch (material->GetMaterialType())
			{
			case MaterialType::Skybox:
			{
				material->GetMaterialData().Shader_ = Shader::Create(material->GetShaderPath());
				material->GetMaterialData().Texture_ = Texture2D::Create(material->GetSkyboxTexturePaths());
				material->GetMaterialData().VertexArray_ = VertexArray::Create();
				material->GetMaterialData().VertexBuffer_ = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices));
				material->GetMaterialData().VertexBuffer_->SetLayout
				({
					{ShaderDataType::Float3, "a_Position"}
				});
				material->GetMaterialData().VertexArray_->AddVertexBuffer(material->GetMaterialData().VertexBuffer_);
				material->GetMaterialData().Shader_->Bind();
				material->GetMaterialData().Shader_->SetUniformInt1("skybox", 0);
				break;
			}
			case MaterialType::Light:
			{
				material->GetMaterialData().Shader_ = Shader::Create(material->GetShaderPath());
				material->GetMaterialData().VertexArray_ = VertexArray::Create();
				material->GetMaterialData().VertexBuffer_ = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
				material->GetMaterialData().VertexBuffer_->SetLayout
				({
					{ShaderDataType::Float3, "a_Position"},
					{ShaderDataType::Float2, "a_TexCoord"},
					{ShaderDataType::Float3, "a_Normals"}
					});
				material->GetMaterialData().VertexArray_->AddVertexBuffer(material->GetMaterialData().VertexBuffer_);
				material->GetMaterialData().Shader_->Bind();
				break;
			}
			case MaterialType::Cube:
			{
				material->GetMaterialData().Shader_ = Shader::Create(material->GetShaderPath());
				if(material->TextureEnabled() && material->GetMaterialData().TexturePath != "")
					material->GetMaterialData().Texture_ = Texture2D::Create(material->GetTexturePath());
				if (material->TextureEnabled() && material->GetMaterialData().TexturePath2 != "")
					material->GetMaterialData().Texture2_ = Texture2D::Create(material->GetTexturePath2());
				material->GetMaterialData().VertexArray_ = VertexArray::Create();
				material->GetMaterialData().VertexBuffer_ = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
				material->GetMaterialData().VertexBuffer_->SetLayout
				({
					{ShaderDataType::Float3, "a_Position"},
					{ShaderDataType::Float2, "a_TexCoord"},
					{ShaderDataType::Float3, "a_Normals"}
				});
				material->GetMaterialData().VertexArray_->AddVertexBuffer(material->GetMaterialData().VertexBuffer_);
				material->GetMaterialData().IndexBuffer_ = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices));
				material->GetMaterialData().VertexArray_->SetIndexBuffer(material->GetMaterialData().IndexBuffer_);
				material->GetMaterialData().Shader_->Bind();
				material->GetMaterialData().Shader_->SetUniformInt1("material.u_DiffuseMap", 0);
				material->GetMaterialData().Shader_->SetUniformInt1("material.u_SpecularMap", 1);
				break;
			}
			case MaterialType::Model:
				break;
			case MaterialType::Sprite:
				break;
			case MaterialType::Sphere:
				break;
			case MaterialType::Line:
			{
				lineVertices[0] = material->GetMaterialData().Point1.x;
				lineVertices[1] = material->GetMaterialData().Point1.y;
				lineVertices[2] = material->GetMaterialData().Point1.z;

				lineVertices[3] = material->GetMaterialData().Point2.x;
				lineVertices[4] = material->GetMaterialData().Point2.y;
				lineVertices[5] = material->GetMaterialData().Point2.z;

				material->GetMaterialData().Shader_ = Shader::Create(material->GetShaderPath());
				material->GetMaterialData().VertexArray_ = VertexArray::Create();
				material->GetMaterialData().VertexBuffer_ = VertexBuffer::Create(lineVertices, sizeof(lineVertices));
				material->GetMaterialData().VertexBuffer_->SetLayout
				({
					{ShaderDataType::Float3, "a_Position"}
					});
				material->GetMaterialData().VertexArray_->AddVertexBuffer(material->GetMaterialData().VertexBuffer_);
				break;
			}
			default:
				Albedo_Core_ERROR("Error: Invalid type");
				break;
			}
		}
	}

	void Renderer::Setup(const EditorCamera& camera, const Material& material)
	{
		if (material.GetMaterialType() == MaterialType::Skybox)
		{
			//glDepthMask(GL_FALSE);
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			material.GetMaterialData().Shader_->Bind();
			glm::mat4 projection = camera.GetProjection();
			glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
			material.GetMaterialData().Shader_->SetUniformMat4("view", view);
			material.GetMaterialData().Shader_->SetUniformMat4("projection", projection);

			glBindVertexArray(material.GetMaterialData().VertexArray_->GetRendererID());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, material.GetMaterialData().Texture_->GetTextureID());
		}
		else if (material.GetMaterialType() == MaterialType::Light)
		{
			material.GetMaterialData().Shader_->Bind();
			material.GetMaterialData().Shader_->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
			material.GetMaterialData().Shader_->SetUniformFloat4("u_MaterialColor", material.GetMaterialData().Color);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), material.GetMaterialData().LightPos)
				* glm::scale(glm::mat4(1.0f), material.GetMaterialData().Scale);
			material.GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);

			material.GetMaterialData().VertexArray_->Bind();
		}
		else if (material.GetMaterialType() == MaterialType::Cube)
		{
			material.GetMaterialData().Shader_->Bind();
			material.GetMaterialData().Shader_->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
			material.GetMaterialData().Shader_->SetUniformFloat3("light.u_Position", material.GetMaterialData().LightPos);

			material.GetMaterialData().Shader_->SetUniformFloat3("u_CameraPos", camera.GetPosition());

			material.GetMaterialData().Shader_->SetUniformFloat3("light.u_Ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
			material.GetMaterialData().Shader_->SetUniformFloat3("light.u_Diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
			material.GetMaterialData().Shader_->SetUniformFloat3("light.u_Specular", glm::vec3(1.0f, 1.0f, 1.0f));

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), material.GetMaterialData().Position)
				* glm::scale(glm::mat4(1.0f), material.GetMaterialData().Scale);
			material.GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);

			// material properties
			material.GetMaterialData().Shader_->SetUniformFloat("material.u_Shininess", 64.0f);

#if 0
			material.GetMaterialData().Shader_->Bind();
			material.GetMaterialData().Shader_->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
			material.GetMaterialData().Shader_->SetUniformFloat3("u_MaterialColor", material.GetMaterialData().Color);
			material.GetMaterialData().Shader_->SetUniformFloat3("u_LightColor", material.GetMaterialData().LightColor);
			material.GetMaterialData().Shader_->SetUniformFloat3("u_LightPos", material.GetMaterialData().LightPos);
			material.GetMaterialData().Shader_->SetUniformFloat3("u_CameraPos", camera.GetPosition());
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), material.GetMaterialData().Position)
				* glm::scale(glm::mat4(1.0f), material.GetMaterialData().Scale);
			material.GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);
			material.GetMaterialData().Shader_->SetUniformInt1("u_TextureEnabled", material.TextureEnabled());
#endif

			material.GetMaterialData().VertexArray_->Bind();
			if(material.GetMaterialData().Texture_)
				material.GetMaterialData().Texture_->Bind(0);
			if (material.GetMaterialData().Texture2_)
				material.GetMaterialData().Texture2_->Bind(1);
		}
		else if (material.GetMaterialType() == MaterialType::Line)
		{
			glEnable(GL_LINE_SMOOTH);

			material.GetMaterialData().Shader_->Bind();
			material.GetMaterialData().Shader_->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
			material.GetMaterialData().Shader_->SetUniformFloat4("u_Color", material.GetMaterialData().Color);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), material.GetMaterialData().Position)
				* glm::scale(glm::mat4(1.0f), material.GetMaterialData().Scale);
			material.GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);

			material.GetMaterialData().VertexArray_->Bind();
		}

		//s_RendererData.ModelShader = Shader::Create("Assets/ModelShader.glsl");
		//s_RendererData.SuzzaneModel = new Model("Assets/backpack/backpack.obj");
	}

	void Renderer::Render(const Material& material)
	{
		if (material.GetMaterialType() == MaterialType::Skybox)
		{
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDepthMask(GL_TRUE);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS);
		}
		else if (material.GetMaterialType() == MaterialType::Light)
		{
			material.GetMaterialData().VertexArray_->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
		}
		else if (material.GetMaterialType() == MaterialType::Cube)
		{
			material.GetMaterialData().VertexArray_->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
		}
		else if (material.GetMaterialType() == MaterialType::Line)
		{
			material.GetMaterialData().VertexArray_->Bind();
			glLineWidth(material.GetMaterialData().LineWidth);
			glDrawArrays(GL_LINES, 0, 2);
		}
	}

	void Renderer::Shutdown()
	{

	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		Albedo_PROFILE_FUNCTION();
		RenderCommand::SetViewPort(720, 1280, width, height);
	}

	void Renderer::DrawModel()
	{
		//s_RendererData.SuzzaneModel->Draw(s_RendererData.ModelShader);
	}
}