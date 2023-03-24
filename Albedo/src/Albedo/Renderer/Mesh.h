#pragma once

#include "Texture.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Albedo/Core/Core.h"

#define MAX_BONE_INFLUENCE 4

namespace Albedo {

    struct Vertex 
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        //glm::vec3 Tangent;
        //glm::vec3 Bitangent;
        //int m_BoneIDs[MAX_BONE_INFLUENCE];
        //float m_Weights[MAX_BONE_INFLUENCE];

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
        Mesh() = default;
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<MeshTexture> textures);
		~Mesh();

		void InitMesh();
        void Render(Ref<Shader> shader);

        const std::vector<Ref<Texture2D>>& GetTextures() const { return m_textures; }

        void SetIndices(const std::vector<uint32_t>& indices) { m_indices = indices; }
        void SetVertices(const std::vector<glm::vec3>& vertices) { m_vertices = vertices; }
        void SetNormals(const std::vector<glm::vec3>& normals) { m_normals = normals; }
        void SetUV(const std::vector<glm::vec2>& uv) { m_uv = uv; }
	private:
        std::vector<Ref<Texture2D>> m_textures;
        std::vector<glm::vec3> m_vertices;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec2> m_uv;
        std::vector<uint32_t>  m_indices;

		uint32_t m_VAO;
		uint32_t m_VBO;
		uint32_t m_TBO;
		uint32_t m_NBO;
		uint32_t m_EBO;
	};

}