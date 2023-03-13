#pragma once

#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"

namespace Albedo {

	struct MaterialData
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec4 Color = { 1.0f, 0.0f, 0.0f, 1.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		std::string ShaderPath = "";
		std::string TexturePath = "";
		std::vector<std::string> SkyboxTexturesPaths;
		float* VertexData;
		uint32_t* IndexData;
		Ref<VertexArray> VertexArray_;
		Ref<VertexBuffer> VertexBuffer_;
		Ref<Shader> Shader_;
		Ref<Texture2D> Texture_;
	};

	enum class MaterialType
	{
		Skybox = 0,
		Cube,
		Model,
		Sprite,
		Sphere,
		Line

	};

	class Material
	{
	public:
		Material();
		~Material();

		void Init(MaterialType type);
		void InitSkybox();
		void AddShader(const std::string& shaderPath) { m_MaterialData.ShaderPath = shaderPath; }
		void AddTexture(const std::string& texturePath) { m_MaterialData.TexturePath = texturePath; }
		void AddSkyboxTexture(const std::vector<std::string>& skyboxTexturePaths) { m_MaterialData.SkyboxTexturesPaths = skyboxTexturePaths; }
		void ChangeTexture(const std::string& texturePath){ m_MaterialData.TexturePath = texturePath; }
		void AddVertexData();
		void AddIndexData();
		void ChangePosition(glm::vec3& position) { m_MaterialData.Position = position; }
		void ChangeColor(glm::vec4& color) { m_MaterialData.Color = color; }
		void ChangeScale(glm::vec3& scale) { m_MaterialData.Scale = scale; }

		inline const std::string&				GetShaderPath()			const { return m_MaterialData.ShaderPath;			}
		inline const std::string&				GetTexturePath()		const { return m_MaterialData.TexturePath;			}
		inline const std::vector<std::string>&  GetSkyboxTexturePaths() const { return m_MaterialData.SkyboxTexturesPaths;  }
		inline const float*						GetVertexData()			const { return m_MaterialData.VertexData;			}
		inline const uint32_t*					GetIndexData()			const { return m_MaterialData.IndexData;			}
		inline const uint32_t					GetVertexSize()			const { return sizeof(m_MaterialData.VertexData);	}
		inline const Ref<VertexArray>			GetVertexArray()		const { return m_MaterialData.VertexArray_;			}
		inline const Ref<VertexBuffer>			GetVertexBuffer()		const { return m_MaterialData.VertexBuffer_;		}
		inline const Ref<Shader>				GetShader()				const { return m_MaterialData.Shader_;				}
		inline const Ref<Texture2D>				GetTexture()			const { return m_MaterialData.Texture_;				}
		inline const MaterialType&				GetMaterialType()		const { return m_Type;								}
		inline const MaterialData&				GetMaterialData()		const { return m_MaterialData;						}
		// non const
		inline		 MaterialData&				GetMaterialData()			  { return m_MaterialData;						}
		inline		 Ref<VertexArray>			GetVertexArray()			  { return m_MaterialData.VertexArray_;			}
		inline		 Ref<VertexBuffer>			GetVertexBuffer()			  { return m_MaterialData.VertexBuffer_;		}
		inline		 Ref<Shader>				GetShader()					  { return this->m_MaterialData.Shader_;				}
		inline		 Ref<Texture2D>				GetTexture()				  { return m_MaterialData.Texture_;				}

		inline void Show(bool visibility) { m_Show = visibility; };
	private:
		void setVertexData();
		void SetIndexData();
	private:
		MaterialData m_MaterialData;
		MaterialType m_Type;
		bool m_Show;
	};

}