#pragma once

#include "Platform/OpenGL/OpenGLShader.h"

#define MAX_BONE_INFLUENCE 4

namespace Albedo {

    struct Vertex 
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        float m_Weights[MAX_BONE_INFLUENCE];

        auto operator= (const std::vector<Vertex> other)
        {
            return other;
        }
    };

    struct MeshTexture
    {
        uint32_t id;
        std::string type;
        std::string path;
    };

	class Mesh 
    {
	public:
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<MeshTexture> textures);
		~Mesh();

		void InitMesh();
		void Render(Ref<Shader> shader);
	private:
		std::vector<Vertex>     m_Vertices;
		std::vector<uint32_t>	m_Indices;
		std::vector<MeshTexture>    m_Textures;
		uint32_t m_VAO;
		uint32_t m_VBO;
		uint32_t m_EBO;
	};

}