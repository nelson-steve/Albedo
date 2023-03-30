#pragma once

#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"

namespace Albedo {

	enum class MaterialType
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
	private:
		MaterialType m_MaterialType;

	};

}