#pragma once
#include <Albedo/Utils/AssetSystem.h>

namespace Albedo{
	
	class SceneObject
	{
	public:
		void AddMesh	 (const Ref<Mesh>     mesh)		{ m_Mesh = mesh;				   }
		void AddTexture  (const Ref<Texture>  texture)  { m_Textures.push_back(texture);   }
		void AddShader   (const Ref<Shader>   shader)   { m_Shader = shader;			   }
		void AddMaterial (const Ref<Material> material) { m_Material = material;		   }
		void AddPostion  (const glm::vec3& pos)			{ m_Position = pos;		   }

		Ref<Mesh>					     GetMesh()	   const { return m_Mesh;			   }
		const std::vector<Ref<Texture>>& GetTextures() const { return m_Textures;		   }
		Ref<Shader>						 GetShader()   const { return m_Shader;			   }
		Ref<Material>					 GetMaterial() const { return m_Material;	       }
		const glm::vec3&				 GetPosition() const { return m_Position;		   }
	private:
		Ref<Mesh>				   m_Mesh;
		std::vector<Ref<Texture>>  m_Textures;
		Ref<Shader>				   m_Shader;
		Ref<Material>			   m_Material;
		glm::vec3 m_Position;
	};

}