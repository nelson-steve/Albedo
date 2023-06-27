#include "AlbedoPreCompiledHeader.h"
#include "TerrainChunk.h"
#include "Platform/OpenGL/Utils.h"

#include <stb_image.h>

namespace Albedo {

	TerrainChunk::TerrainChunk()
	{
	}

	void TerrainChunk::Init()
	{
		m_Shader = Shader::Create("Assets/Shaders/Terrain.glsl");

		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load image, create texture and generate mipmaps
		int width, height, nrChannels;
		// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
		unsigned char* data = stbi_load("Assets/Textures/iceland_heightmap.png", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		
			m_Shader->SetUniformInt1("heightMap", 0);
			std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);


		//m_VAO = VertexArray::Create();

		//m_GrassTexture = Texture2D::Create("Assets/Textures/Diluc.png", false);
		//m_StoneTexture = Texture2D::Create("Assets/Textures/Diluc.png", false);
		//m_Normalmap = Texture2D::Create("Assets/Textures/Diluc.png", false);
		m_HeightmapGenerator = std::make_shared<HeightmapGenerator>();
		//m_Heightmap = m_HeightmapGenerator->GetTexture();
		//m_HeightmapGenerator->InitPerlinNoiseTexture();
		//m_HeightmapGenerator->GeneratePerlinNoiseTexture(10);
		m_HeightmapGenerator->Init();
		m_Heightmap = m_HeightmapGenerator->GenerateHeightmap();
		//uint32_t width = m_Heightmap->getWidth();
		//uint32_t height = m_Heightmap->getHeight();
		{
			uint32_t size = 4 * 512 * 512;
			m_Heightmap->Bind();
			float* data = new float[size];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);

			Albedo_Core_INFO("sdfsd");

			m_Heightmap = Texture2D::Create(512, 512);
			m_Heightmap->SetData(data, sizeof(data));
		}


		//delete data;

		for (unsigned i = 0; i <= rez - 1; i++)
		{
			for (unsigned j = 0; j <= rez - 1; j++)
			{
				m_Vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
				m_Vertices.push_back(0.0f); // v.y
				m_Vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
				m_Vertices.push_back(i / (float)rez); // u
				m_Vertices.push_back(j / (float)rez); // v

				m_Vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
				m_Vertices.push_back(0.0f); // v.y
				m_Vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
				m_Vertices.push_back((i + 1) / (float)rez); // u
				m_Vertices.push_back(j / (float)rez); // v

				m_Vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
				m_Vertices.push_back(0.0f); // v.y
				m_Vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
				m_Vertices.push_back(i / (float)rez); // u
				m_Vertices.push_back((j + 1) / (float)rez); // v

				m_Vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
				m_Vertices.push_back(0.0f); // v.y
				m_Vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
				m_Vertices.push_back((i + 1) / (float)rez); // u
				m_Vertices.push_back((j + 1) / (float)rez); // v
			}
		}

		glGenVertexArrays(1, &terrainVAO);
		glBindVertexArray(terrainVAO);

		glGenBuffers(1, &terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_Vertices.size(), &m_Vertices[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texCoord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);

		uint32_t numOfPatchPoints = 4;
		Utils::PatchParameter(numOfPatchPoints);
		//m_VAO = VertexArray::Create();
		//m_VBO = VertexBuffer::Create(m_Vertices, Utils::GetVertexSize(m_Vertices));
		//m_VBO->SetLayout
		//({ 
		//	{ShaderDataType::Float3, "aPos"},
		//	{ShaderDataType::Float2, "aTex"}
		//});
		//m_VAO->AddVertexBuffer(m_VBO);

		//Utils::PatchParameter(4);
	}

	void TerrainChunk::SetShaderUniforms(const EditorCamera& camera)
	{
		m_Shader->Bind();
		m_Shader->SetUniformMat4("view", camera.GetViewMatrix());
		m_Shader->SetUniformMat4("projection", camera.GetProjection());
		glm::mat4 transform = glm::mat4(1.0);
		m_Shader->SetUniformMat4("model", transform);
		//m_Shader->SetUniformFloat3("viewPos", camera.GetPosition());

		//m_Shader->SetUniformInt1("heightMap", m_Heightmap->GetTextureID());

		//m_Shader->SetUniformInt1("min_tess_level", m_MinTessLevel);
		//m_Shader->SetUniformInt1("max_tess_level", m_MaxTessLevel);
		//m_Shader->SetUniformInt1("heightMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		//glBindTexture(GL_TEXTURE_2D, m_Heightmap->GetTextureID());

		//if(m_Heightmap)
		//m_Heightmap->Bind();
		//m_GrassTexture->Bind(1);
		//m_StoneTexture->Bind(2);
	}

	void TerrainChunk::Update()
	{
		//m_Heightmap = m_HeightmapGenerator->GenerateHeightmap();
	}

	void TerrainChunk::Render(const EditorCamera& camera)
	{
		m_Shader->Bind();
		m_Shader->SetUniformMat4("view", camera.GetViewMatrix());
		m_Shader->SetUniformMat4("projection", camera.GetProjection());
		glm::mat4 transform = glm::mat4(1.0);
		m_Shader->SetUniformMat4("model", transform);
		//m_Shader->SetUniformFloat3("viewPos", camera.GetPosition());

		//m_Shader->SetUniformInt1("heightMap", m_Heightmap->GetTextureID());

		//m_Shader->SetUniformInt1("min_tess_level", m_MinTessLevel);
		//m_Shader->SetUniformInt1("max_tess_level", m_MaxTessLevel);
		//m_Shader->SetUniformInt1("heightMap", 0);

		m_Heightmap->Bind();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture);

		//if(m_Heightmap)
		//m_Heightmap->Bind();
		//m_GrassTexture->Bind(1);
		//m_StoneTexture->Bind(2);

		glBindVertexArray(terrainVAO);
		//m_VAO->Bind();
		Utils::PatchParameter(4);
		glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
		//glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
		//glDrawArrays(GL_PATCHES, 0, m_TerrainGeneration->NUM_PATCH_PTS * m_TerrainGeneration->rez * m_TerrainGeneration->rez);
	}

	void TerrainChunk::LoadHeightmapTexture(int width, int height)
	{
		//m_HeightmapGenerator = std::make_shared<HeightmapGenerator>(width, height);
		//m_HeightmapGenerator->InitPerlinNoiseTexture(5, 2, 10, 4, 3, 10, 20);
		//m_HeightmapGenerator->GeneratePerlinNoiseTexture(19);
		//m_TextureHeightmap = m_HeightmapGenerator->GetTexture();
	}

}

