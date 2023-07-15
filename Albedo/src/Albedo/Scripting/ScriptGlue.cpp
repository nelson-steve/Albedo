#include "AlbedoPreCompiledHeader.h"
#include "ScriptGlue.h"

#include "Albedo/Scene/Scene.h"
#include "ScriptEngine.h"
#include "Albedo/Core/KeyCodes.h"
#include "Albedo/Core/Input.h"
#include "Albedo/Scene/Components.h"

#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

namespace Albedo {

	static std::unordered_map<MonoType*, std::function<bool(entt::entity)>> s_EntityHasComponentFuncs;

#define HZ_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Albedo.InternalCalls::" #Name, Name)

	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		Albedo_Core_WARN("Value: {0} {1} {2}", glm::vec3(*parameter).x, glm::vec3(*parameter).y, glm::vec3(*parameter).z);
		*outResult = glm::normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		Albedo_Core_WARN("Value: {0} {1} {2}", glm::vec3(*parameter).x, glm::vec3(*parameter).y, glm::vec3(*parameter).z);
		return glm::dot(*parameter, *parameter);
	}

	static MonoObject* GetScriptInstance(uint32_t entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}

	static bool Entity_HasComponent(uint32_t entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Albedo_CORE_ASSERT(scene, "coudn't get the scene context");
		entt::entity entity = static_cast<entt::entity>(entityID);
		
		//Albedo_CORE_ASSERT(entity, "Invalid entity");

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		Albedo_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), "Component not found");
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::GetSceneContext();
		Albedo_CORE_ASSERT(scene, "failed to load scene");
		Entity entity = scene->FindEntityByName(nameCStr);
		mono_free(nameCStr);

		if (!entity)
			return 0;

		return (uint64_t)entity.GetEntityHandle();
	}

	static void TransformComponent_GetTranslation(uint32_t entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Albedo_CORE_ASSERT(scene, "count't get the scene context");
		entt::entity entity = static_cast<entt::entity>(entityID);
		Albedo_CORE_ASSERT(entity != entt::null, "invalid entity");

		*outTranslation = Entity::GetComponent<TransformComponent>(entity).Position;
	}

	static void TransformComponent_SetTranslation(uint32_t entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Albedo_CORE_ASSERT(scene, "count't get the scene context");
		entt::entity entity = static_cast<entt::entity>(entityID);
		Albedo_CORE_ASSERT(entity != entt::null, "invalid entity");

		Entity::GetComponent<TransformComponent>(entity).Position = *translation;
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(uint32_t entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Albedo_CORE_ASSERT(scene, "couns't get scene context");
		entt::entity entity = static_cast<entt::entity>(entityID);
		Albedo_CORE_ASSERT(entity != entt::null, "invalid entity");

		auto& rb2d = Entity::GetComponent<Physics2DComponent>(entity);
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(uint32_t entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Albedo_CORE_ASSERT(scene, "couldn't get scene context");
		entt::entity entity = static_cast<entt::entity>(entityID);
		Albedo_CORE_ASSERT(entity != entt::null, "invalid entity");

		auto& rb2d = Entity::GetComponent<Physics2DComponent>(entity);;
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template<typename... Component>	
	static void RegisterComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("Albedo.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					Albedo_Core_ERROR("Could not find component type {}", managedTypename);
					return;
				}
				s_EntityHasComponentFuncs[managedType] = [](entt::entity entity) { return Entity::HasComponent<Component>(entity); };
			}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		HZ_ADD_INTERNAL_CALL(NativeLog);
		HZ_ADD_INTERNAL_CALL(NativeLog_Vector);
		HZ_ADD_INTERNAL_CALL(NativeLog_VectorDot);

		HZ_ADD_INTERNAL_CALL(GetScriptInstance);

		HZ_ADD_INTERNAL_CALL(Entity_HasComponent);
		HZ_ADD_INTERNAL_CALL(Entity_FindEntityByName);

		HZ_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		HZ_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

		HZ_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}

}