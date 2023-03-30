#pragma once

#include "Texture.h"
#include "Buffer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Albedo/Core/Core.h"
#include "VertexArray.h"

#define MAX_BONE_INFLUENCE 4

namespace Albedo {

    struct MeshBufferData
    {
        std::vector<uint32_t> m_TextureIDs;
        Ref<VertexArray>  m_VertexArray;
        Ref<VertexBuffer> m_VertexBuffer;
        Ref<VertexBuffer> m_UVBuffer;
        Ref<VertexBuffer> m_NormalBuffer;
        Ref<VertexBuffer> m_InstanceBuffer;
    };

    class Mesh
    {
    public:
        void InitMesh();
        const std::string&    GetName()           const { return m_Name;           }
        const std::string&    GetPath()           const { return m_Path;           }
        const MeshBufferData& GetMeshBufferData() const { return m_MeshBufferData; }
		const std::vector<glm::vec3> GetVertices() const { return m_Vertices;	   }

        void SetName                  (const std::string& name)                { m_Name             = name;     }
        void SetUV                    (const std::vector<glm::vec2>& uv)       { m_UV               = uv;       }
        void SetIndices               (const std::vector<uint32_t>& indices)   { m_Indices          = indices;  }
        void SetVertices              (const std::vector<glm::vec3>& vertices) { m_Vertices         = vertices; }
        void SetNormals               (const std::vector<glm::vec3>& normals)  { m_Normals          = normals;  }
		void SetSingularMeshData	  (const std::vector<float>& data)		   { m_SingularMeshData = data;		}
        void SetVerticesDataLayout    (const BufferLayout& layout)             { m_Layout           = layout;   }
        void SetDataSingularityStatus (bool status)                            { m_SingularData     = status;   }

        bool IsSingularData() const { return m_SingularData; }
	private:
		//Get the size of the vertex data for the Vertex Buffer
		template<typename T>
		uint32_t GetVertexSize(const std::vector<T> data)
		{
			return data.size() * sizeof(T);
		}
    private:
        std::string m_Name = "Mesh"; //default
        std::string m_Path;
        std::vector<glm::vec3> m_Vertices;
        std::vector<glm::vec3> m_Normals;
        std::vector<glm::vec2> m_UV;
        std::vector<uint32_t>  m_Indices;
        //Contains all the mesh data in one variable (vertices, normals, uvs...)
		std::vector<float> m_SingularMeshData;
        //Layout for singular vertices data
        BufferLayout m_Layout;
        bool m_SingularData = true;
        MeshBufferData m_MeshBufferData;
    };

}