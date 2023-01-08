#include "AlbedoPreCompiledHeader.h"

#include "Entity.h"

namespace Albedo {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

}