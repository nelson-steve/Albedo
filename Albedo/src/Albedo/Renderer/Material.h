#pragma once

#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"

namespace Albedo {

	enum MaterialType
	{
		AlbedoMap = 0,
		NormalMap,
		TextureMap,
		LightingMap,
		ShadowMap
	};

	class Material
	{
	public:
		void SetAlbedo();
		void SetRoughness();
		void SetMaterialColor();
		void SetMaterialType(MaterialType matType);

		const std::vector<Ref<Texture2D>>& GetTextures() const { return m_Textures; }
		std::vector<Ref<Texture2D>>& GetTextures() { return m_Textures; }
	private:
		MaterialType m_MaterialType;
		std::vector<Ref<Texture2D>>  m_Textures;

	};

}