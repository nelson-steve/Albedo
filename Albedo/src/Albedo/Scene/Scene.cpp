#include "AlbedoPreCompiledHeader.h"

#include "Scene.h"
#include "Entity.h"

#include "Components.h"
#include "Albedo/Renderer/BatchRenderer2D.h"

#include <glm/glm.hpp>

#define ENTT_EXAMPLE_CODE 0

namespace Albedo {

	Scene::Scene()
	{
	#if ENTT_EXAMPLE_CODE
		entt::entity entity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		
		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();
		auto v = m_Registry.get<TransformComponent>(entity);
		//if (v)
		TransformComponent& transform = m_Registry.get<TransformComponent>(entity);


		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}
	#endif

	}

	Scene::~Scene()
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::OnUpdate(Timestep ts)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			BatchRenderer2D::DrawQuad(transform, sprite.Color);
		}
	}

}