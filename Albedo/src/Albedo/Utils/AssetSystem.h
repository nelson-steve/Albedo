#pragma once

#include "Albedo/Renderer/Texture.h"
#include <Albedo/Renderer/Mesh.h>

namespace Albedo {
	
	class AssetSystem
	{
	public:
		AssetSystem();
		~AssetSystem();

		void Init();

		const std::shared_ptr<Mesh> LoadMeshes();
		void DefaultCube();
		void DefaultQuad();

		const std::shared_ptr<Texture> LoadTexture();

		void LoadShader();
	private:
		std::shared_ptr<Texture> m_Textures;
		std::shared_ptr<Mesh> m_Meshes;
	};

}