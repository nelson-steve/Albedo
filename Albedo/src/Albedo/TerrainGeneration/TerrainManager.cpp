#include "AlbedoPreCompiledHeader.h"

#include "TerrainManager.h"
#include "Platform/OpenGL/Utils.h"
#include "Albedo/Utils/ImageLoader.h"

namespace Albedo {

	TerrainManager::TerrainManager()
	{
		m_LandTexture = Texture2D::Create("Assets/Textures/grass.jpg", false);

		m_Shader = Shader::Create("Assets/Shaders/TerrainMVP.glsl");

		m_NoiseMap = std::make_shared<HeightmapGenerator>(512, 512);
		m_NoiseMap->SetScale(220.0);

		m_NoiseMap->GenerateValues();
		m_NoiseMap->UpdateTexture();
		glm::uvec2 size{512, 512};
		m_Terrain = std::make_shared<Terrain>(size, m_NoiseMap->GetHeightMapData());

		std::vector<std::string> terrainTextures;

		terrainTextures.push_back("Assets/Textures/water.jpg");
		terrainTextures.push_back("Assets/Textures/sand.jpg");
		terrainTextures.push_back("Assets/Textures/grass.jpg");
		terrainTextures.push_back("Assets/Textures/stonyGrass.jpg");
		terrainTextures.push_back("Assets/Textures/rocky.jpg");
		terrainTextures.push_back("Assets/Textures/mountains.jpg");
		terrainTextures.push_back("Assets/Textures/snow.jpg");

		std::vector<Image> images;
		for (const auto& tex : terrainTextures)
		{
			images.push_back(Image(tex));
		}

		std::vector<const void*> data(terrainTextures.size());
		for (uint32_t i = 0; i < terrainTextures.size(); ++i)
		{
			Albedo_CORE_ASSERT(images[i].m_Width <= m_TexArrayTexWidth, "");
			Albedo_CORE_ASSERT(images[i].m_Height <= m_TexArrayTexHeight, "");
			data[i] = static_cast<const void*>(images[i].data);
		}

		m_TexArray = Texture2DArray::Create(m_TexArrayTexWidth, m_TexArrayTexHeight, data);

		const float kTerrainHeightScale = m_Terrain->GetHeightScale();

		m_TerrainUBOData.minHeight = m_NoiseMap->GetMinValue() * kTerrainHeightScale;
		m_TerrainUBOData.maxHeight = m_NoiseMap->GetMaxValue() * kTerrainHeightScale;

		m_TerrainUBOData.regionCount = static_cast<int>(m_Regions.size());

		Albedo_CORE_ASSERT(m_Regions.size() <= s_kMaxRegionCount, "");


		for (uint32_t i = 0; i < m_Regions.size(); ++i)
		{
			auto& region = m_TerrainUBOData.regions[i];

			region.texScale = m_Regions[i].scale;
			region.texIndex = m_Regions[i].texIndex;
			region.tint = m_Regions[i].tint;
			region.tintStrength = m_Regions[i].tintStrength;
			region.blendStrength = m_Regions[i].blendStrength;
			region.startHeight = m_Regions[i].startHeight;
		}

		m_Terrain->GetVAO()->Bind();
		m_TerrainUBO = UniformBuffer::Create(&m_TerrainUBOData, sizeof(TerrainUBO), m_Shader->GetShaderID(), 1);

		m_Shader->Bind();
		m_Shader->SetUniformInt1("u_RegionCount", m_Regions.size());
		m_Shader->SetUniformInt1("u_MinHeight", m_NoiseMap->GetMinValue() * kTerrainHeightScale);
		m_Shader->SetUniformFloat("u_MaxHeight", m_NoiseMap->GetMaxValue() * kTerrainHeightScale);

		Albedo_CORE_ASSERT(m_Regions.size() > 0, "");
		int i = 0;
		for (auto& region : m_Regions)
		{
			m_Shader->SetUniformFloat("regions["+std::to_string(i)+"].scale", m_Regions[i].scale);
			m_Shader->SetUniformInt1("regions["+std::to_string(i)+"].texIndex", m_Regions[i].texIndex);
			m_Shader->SetUniformFloat("regions["+std::to_string(i)+"].blendStrength", m_Regions[i].blendStrength);
			m_Shader->SetUniformFloat("regions["+std::to_string(i)+"].startHeight", m_Regions[i].startHeight);
			m_Shader->SetUniformFloat3("regions["+std::to_string(i)+"].tint", m_Regions[i].tint);
			i++;
		}

#if 0
		int width = 1024;
		int height = 1024;
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

		m_VAO = VertexArray::Create();
		m_VBO = VertexBuffer::Create(m_Vertices, sizeof(float) * m_Vertices.size());
		m_VBO->SetLayout
		({
			{ShaderDataType::Float3, "aPos"},
			{ShaderDataType::Float2, "aTex"}
			});
		m_VAO->AddVertexBuffer(m_VBO);

		Utils::PatchParameter(4);
#endif
	}

	void TerrainManager::Render(const EditorCamera& camera)
	{
		m_Shader->Bind();
		const float kTerrainHeightScale = m_Terrain->GetHeightScale();
		//m_Shader->SetUniformMat4("view", camera.GetViewMatrix());
		m_Shader->SetUniformMat4("MVP", camera.GetProjectionView());
		m_Shader->SetUniformInt1("texArray", 0);
		m_Shader->SetUniformInt1("land", 1);

		//m_TerrainUBO->Bind();

		m_TexArray->Bind();
		m_LandTexture->Bind(1);

		if (m_TerrainChanged)
			UpdateTerrain();

		m_Terrain->Render();

		//for (auto& t : m_TerrainChunks)
		//{
		//	glm::mat4 transform = glm::translate(glm::mat4(1.0), t->GetPosition());
		//	m_Shader->SetUniformMat4("model", transform);
		//	m_Shader->SetUniformInt1("heightMap", 0);
		//	t->GetTHeightMap()->Bind();
		//
		//	m_VAO->Bind();
		//	Utils::PatchParameter(4);
		//	glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
		//}


		//m_Shader->SetUniformFloat3("viewPos", camera.GetPosition());



		//m_Shader->SetUniformInt1("min_tess_level", m_MinTessLevel);
		//m_Shader->SetUniformInt1("max_tess_level", m_MaxTessLevel);
		//m_Shader->SetUniformInt1("heightMap", 0);


		//glBindTexture(GL_TEXTURE_2D, m_Heightmap->GetTextureID());

		//if(m_Heightmap)
		//m_Heightmap->Bind();
		//m_GrassTexture->Bind(1);
		//m_StoneTexture->Bind(2);
	}
	void TerrainManager::UpdateTerrain()
	{
		const float kTerrainHeightScale = m_Terrain->GetHeightScale();

		m_TerrainUBOData.minHeight = m_NoiseMap->GetMinValue() * kTerrainHeightScale;
		m_TerrainUBOData.maxHeight = m_NoiseMap->GetMaxValue() * kTerrainHeightScale;

		m_TerrainUBOData.regionCount = static_cast<int>(m_Regions.size());

		Albedo_CORE_ASSERT(m_Regions.size() <= s_kMaxRegionCount, "");


		for (uint32_t i = 0; i < m_Regions.size(); ++i)
		{
			auto& region = m_TerrainUBOData.regions[i];

			region.texScale = m_Regions[i].scale;
			region.texIndex = m_Regions[i].texIndex;
			region.tint = m_Regions[i].tint;
			region.tintStrength = m_Regions[i].tintStrength;
			region.blendStrength = m_Regions[i].blendStrength;
			region.startHeight = m_Regions[i].startHeight;
		}

		m_TerrainUBO->SetData(&m_TerrainUBOData, sizeof(TerrainUBO));
		m_TerrainChanged = false;
	}

	TerrainManager::Region::Region()
		: startHeight(0.0),
		name(""),
		tint(s_kDefaultColor),
		texIndex(0),
		scale(1.0),
		tintStrength(0.1),
		blendStrength(0.1)
	{

	}

	TerrainManager::Region::Region(int n)
		: Region()
	{
		name = "Region " + std::to_string(n);
	}

	TerrainManager::Region::Region(const std::string& newName)
		: Region()
	{
		name = newName;
	}

	TerrainManager::Region::Region(
		float sh, const std::string& newName, glm::vec3 c, int tID,
		float sc, float tintStr, float blendStr)
		: startHeight(sh),
		name(newName),
		tint(c),
		texIndex(tID),
		scale(sc),
		tintStrength(tintStr),
		blendStrength(blendStr)
	{

	}
}