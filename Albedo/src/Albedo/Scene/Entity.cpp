#include "AlbedoPreCompiledHeader.h"

#include "Entity.h"
#include "Components.h"

#define STATIC(a) decltype(a) a

STATIC(Albedo::Entity::m_Scene);

#undef STATIC(a) decltype(a) a

namespace Albedo {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle)
	{
		m_Scene = scene;
	}

	const std::string& Entity::GetName()
	{
		return GetComponent<TagComponent>().Tag;
	}

}