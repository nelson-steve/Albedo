#pragma once

#include "Albedo/Renderer/Shader.h"
#include "Albedo/Renderer/VertexArray.h"

namespace Albedo {

	class TerrainGeneration
	{
	public:
		TerrainGeneration() = default;

		void Init(Ref<Shader> shader);


		

		unsigned int texture;
		unsigned int terrainVAO, terrainVBO, terrainIBO;
		int width, height, nrChannels;
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		int numStrips;
		int numTrisPerStrip;
		float yScale = 64.0f / 256.0f, yShift = 16.0f;

		const unsigned int NUM_PATCH_PTS = 4;
	};

}