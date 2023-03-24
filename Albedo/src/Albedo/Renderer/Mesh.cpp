#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"
#include "Mesh.h"

#include <glad/glad.h>

namespace Albedo {

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<MeshTexture> textures)
    {
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        InitMesh();
    }

    Mesh::~Mesh()
    {

    }

    void Mesh::InitMesh()
    {
        Ref<Texture2D> tex = Texture2D::Create("Assets/suzanne/albedo.png");
        m_textures.push_back(tex);
        tex.reset();

        tex = Texture2D::Create("Assets/suzanne/ao.png");
        m_textures.push_back(tex);
        tex.reset();

        tex = Texture2D::Create("Assets/suzanne/metallic.png");
        m_textures.push_back(tex);
        tex.reset();

        tex = Texture2D::Create("Assets/suzanne/normal.png");
        m_textures.push_back(tex);
        tex.reset();

        tex = Texture2D::Create("Assets/suzanne/roughness.png");
        m_textures.push_back(tex);
        tex.reset();

        Renderer::ModelShader = Shader::Create("Assets/ModelPBRShader.glsl");
        // create buffers/arrays
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_TBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(m_vertices[0]), &m_vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.

        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        // vertex normals

        glBindBuffer(GL_ARRAY_BUFFER, m_NBO);
        glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, m_TBO);
        glBufferData(GL_ARRAY_BUFFER, m_uv.size() * sizeof(m_uv[0]), &m_uv[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        // vertex texture coords
        //glEnableVertexAttribArray(2);
        //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex tangent
        //glEnableVertexAttribArray(3);
        //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        //// vertex bitangent
        //glEnableVertexAttribArray(4);
        //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        //// ids
        //glEnableVertexAttribArray(5);
        //glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
        //
        //// weights
        //glEnableVertexAttribArray(6);
        //glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }

    void Mesh::Render(Ref<Shader> shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        //for (unsigned int i = 0; i < m_Textures.size(); i++)
        //{
        //    glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        //    // retrieve texture number (the N in diffuse_textureN)
        //    std::string number;
        //    std::string name = m_Textures[i].type;
        //    if (name == "texture_diffuse")
        //        number = std::to_string(diffuseNr++);
        //    else if (name == "texture_specular")
        //        number = std::to_string(specularNr++); // transfer unsigned int to string
        //    else if (name == "texture_normal")
        //        number = std::to_string(normalNr++); // transfer unsigned int to string
        //    else if (name == "texture_height")
        //        number = std::to_string(heightNr++); // transfer unsigned int to string
        //
        //    // now set the sampler to the correct texture unit
        //    glUniform1i(glGetUniformLocation(shader->GetShaderID(), (name + number).c_str()), i);
        //    // and finally bind the texture 
        //    glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);
        //}

        // draw mesh
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
        //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_Indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);   
    }

}