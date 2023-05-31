#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"
#include "Mesh.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Albedo {
    void Mesh::InitMesh(int id)
    {
        if (!m_Path.empty())
        {
            if (m_Path.find('/') != std::string::npos)
            {
                size_t pos = m_Path.find_last_of('/');
                m_Name = m_Path.substr(pos + 1, m_Path.size());
            }
            else if (m_Path.find('\\') != std::string::npos)
            {
                size_t pos = m_Path.find_last_of('\\');
                m_Name = m_Path.substr(pos + 1, m_Path.size());
            }
            else
            {
                m_Name = m_Path;
            }
        }
        if (m_SingularData)
        {
            m_MeshBufferData.m_VertexArray = VertexArray::Create();
            auto x = sizeof(m_SingularMeshData);
            auto y = GetVertexSize(m_SingularMeshData);
            m_MeshBufferData.m_VertexBuffer = VertexBuffer::Create(m_SingularMeshData, GetVertexSize(m_SingularMeshData));
            m_MeshBufferData.m_VertexBuffer->SetLayout(m_Layout);
            m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_VertexBuffer);

            //std::vector<int> idBuffer;
            //auto size = sizeof(m_SingularMeshData);
            //for (int i = 0; i < m_SingularMeshData.size(); i++)
            //{
            //    idBuffer.push_back(id);
            //}
            //m_MeshBufferData.m_ID = VertexBuffer::Create(idBuffer, GetVertexSize(idBuffer));
            //m_MeshBufferData.m_ID->SetLayout({ {ShaderDataType::Int, "a_ID"} });
            //m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_ID);
        }

        else
        {
            m_MeshBufferData.m_VertexArray = VertexArray::Create();
            m_MeshBufferData.m_VertexArray->Bind();

            if (m_Vertices.size() != 0)
            {
                m_TotalVertices = GetVertexSize(m_Vertices);
                m_MeshBufferData.m_VertexBuffer = VertexBuffer::Create(m_Vertices, GetVertexSize(m_Vertices));
                
                if (m_Indices.size() != 0)
                {
                    m_MeshBufferData.m_InstanceBuffer = VertexBuffer::Create(m_Indices, GetVertexSize(m_Indices));
                    //m_MeshBufferData.m_UVBuffer->SetLayout({ {ShaderDataType::Float2, "a_UV"} });
                    //m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_UVBuffer);
                }

                m_MeshBufferData.m_VertexBuffer->SetLayout({ {ShaderDataType::Float3, "a_Position"} });
                m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_VertexBuffer);
            }

            if (m_Normals.size() != 0)
            {
                m_MeshBufferData.m_NormalBuffer = VertexBuffer::Create(m_Normals, GetVertexSize(m_Normals));
                m_MeshBufferData.m_NormalBuffer->SetLayout({ {ShaderDataType::Float3, "a_Normal"} });
                m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_NormalBuffer);
            }

            if (m_UV.size() != 0)
            {
                m_MeshBufferData.m_UVBuffer = VertexBuffer::Create(m_UV, GetVertexSize(m_UV));
                m_MeshBufferData.m_UVBuffer->SetLayout({ {ShaderDataType::Float2, "a_UV"} });
                m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_UVBuffer);
            }

            //glm::vec3 translations[200];
            std::vector<glm::mat4> positions;
            int index = 0;
            float offset = 1.0f;
            for (int y = -1000; y < 1000; y += 50)
            {
                for (int x = -1000; x < 1000; x += 50)
                {
                    glm::vec3 translation;
                    translation.x = (float)(x + offset) / 10.0f;
                    translation.y = 0.0;
                    translation.z = (float)(y + offset) / 10.0f;
                    glm::mat4 m = glm::translate(glm::mat4(1.0), translation);
                    positions.push_back(m);
                    index++;
                }
            }
            auto count = index;

            m_MeshBufferData.m_Positions = VertexBuffer::Create(positions, GetVertexSize(positions));
            m_MeshBufferData.m_Positions->SetLayout({ {ShaderDataType::Mat4, "m_Positions"} });
            m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_Positions);
            //
            //glBindBuffer(GL_ARRAY_BUFFER, 0);
            //glVertexAttribDivisor(3, 1);

            //std::vector<int> idBuffer;
            //for (int i = 0; i < m_Vertices.size()*3; i++)
            //{
            //    idBuffer.push_back(id);
            //}
            //m_MeshBufferData.m_ID = VertexBuffer::Create(idBuffer, GetVertexSize(idBuffer));
            //m_MeshBufferData.m_ID->SetLayout({ {ShaderDataType::Int, "a_ID"} });
            //m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_ID);


        }
    }

}