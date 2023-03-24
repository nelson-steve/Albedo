#pragma once
#if 0
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.h"

namespace Albedo {

    class Model
    {
    public:
        Model(char* path, bool gamma = false)
        {
            loadModel(path);
        }
        void Draw(Ref<Shader> shader);
    private:
        void loadModel(const std::string& path);
        uint32_t TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<MeshTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    private:
        std::vector<Mesh>    m_Meshes;
        std::string          m_Path;
        std::vector<MeshTexture> m_Textures_loaded;
        bool                 m_GammaCorrection;
    };

}
#endif