#include "AlbedoPreCompiledHeader.h"

#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"

#include <fstream>

#include <yaml-cpp/yaml.h>
#include <Albedo/Core/Application.cpp>

#include <yaml-cpp/yaml.h>

namespace YAML {

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}

namespace Albedo {

#define WRITE_SCRIPT_FIELD(FieldType, Type)           \
			case ScriptFieldType::FieldType:          \
				out << scriptField.GetValue<Type>();  \
				break

#define READ_SCRIPT_FIELD(FieldType, Type)             \
	case ScriptFieldType::FieldType:                   \
	{                                                  \
		Type data = scriptField["Data"].as<Type>();    \
		fieldInstance.SetValue(data);                  \
		break;                                         \
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene> scene)
		:m_Scene(scene)
	{

	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << std::to_string((uint32_t)entity.GetEntityHandle()); // TODO: Entity ID goes here

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;
			auto& mc = entity.GetComponent<MeshComponent>();
			out << YAML::Key << "Color" << YAML::Value << mc.Color;

			auto& mesh = mc.m_Mesh;
			out << YAML::Key << "Mesh" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << mesh->GetName();
			out << YAML::Key << "Path" << YAML::Value << mesh->GetPath();
			out << YAML::EndMap;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TextureComponent>())
		{
			out << YAML::Key << "TextureComponent";
			out << YAML::BeginMap;
			auto& tc = entity.GetComponent<TextureComponent>();

			out << YAML::Key << "Texture" << YAML::Value;
			out << YAML::BeginMap;
			
			out << YAML::Key << "Default Texture" << YAML::Value << tc.m_Textures[tc.TextureType::Default]->GetPath();
		
			out << YAML::Key << "Albedo" << YAML::Value << tc.m_Textures[tc.TextureType::Albedo]->GetPath();
			out << YAML::Key << "Ambient Occlusion" << YAML::Value << tc.m_Textures[tc.TextureType::AmbientOcclusion]->GetPath();
			out << YAML::Key << "Metallic" << YAML::Value << tc.m_Textures[tc.TextureType::Metallic]->GetPath();
			out << YAML::Key << "Normal" << YAML::Value << tc.m_Textures[tc.TextureType::Normal]->GetPath();
			out << YAML::Key << "Roughness" << YAML::Value << tc.m_Textures[tc.TextureType::Roughness]->GetPath();
			
			out << YAML::EndMap;

			out << YAML::Key << "isDefault" << YAML::Value << tc.defaultTexture;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<ShaderComponent>())
		{
			out << YAML::Key << "ShaderComponent";
			out << YAML::BeginMap;
			auto& sc = entity.GetComponent<ShaderComponent>();
			auto& shader = sc.m_Shader;

			out << YAML::Key << "Shader" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "Path" << YAML::Value << shader->GetPath();
			out << YAML::EndMap;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<MaterialComponent>())
		{

		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Position;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<PhysicsComponent>())
		{
			out << YAML::Key << "PhysicsComponent";
			out << YAML::BeginMap; // PhysicsComponent

			auto& tc = entity.GetComponent<PhysicsComponent>();
			out << YAML::Key << "Body Position" << YAML::Value << tc.BodyPosition;
			out << YAML::Key << "Body Orientation" << YAML::Value << glm::vec3(tc.BodyOrientation.x, tc.BodyOrientation.y, tc.BodyOrientation.z);
			out << YAML::Key << "Force" << YAML::Value << tc.Force;
			out << YAML::Key << "Velocity" << YAML::Value << tc.Velocity;
			out << YAML::Key << "Mass" << YAML::Value << tc.Mass;
			out << YAML::Key << "Disable Gravity" << YAML::Value << tc.disableGravity;
			out << YAML::Key << "Restitution" << YAML::Value << tc.restitution;
			out << YAML::Key << "Static Friction" << YAML::Value << tc.staticFriction;
			out << YAML::Key << "Dynamic Friction" << YAML::Value << tc.dynamicFriction;
			
			out << YAML::Key << "Body Type" << YAML::Value << tc.bodyType;

			out << YAML::EndMap; // PhysicsComponent
		}

		if (entity.HasComponent<ColliderComponent>())
		{
			out << YAML::Key << "ColliderComponent";
			out << YAML::BeginMap; // ColliderComponent

			auto& tc = entity.GetComponent<ColliderComponent>();

			out << YAML::Key << "Collider Position" << YAML::Value << tc.ColliderPosition;
			out << YAML::Key << "Collider Size" << YAML::Value << tc.ColliderSize;
			out << YAML::Key << "Collider Orientation" << YAML::Value << glm::vec3(tc.ColliderOrientation.x, tc.ColliderOrientation.y, tc.ColliderOrientation.z);
			out << YAML::Key << "Collider Radius" << YAML::Value << tc.ColliderRadius;

			out << YAML::Key << "Collider Type" << YAML::Value << tc.colliderType;

			out << YAML::EndMap; // ColliderComponent
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			auto& scriptComponent = entity.GetComponent<ScriptComponent>();

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent
			out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;
#if 0
			// Fields
			Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(scriptComponent.ClassName);
			const auto& fields = entityClass->GetFields();
			if (fields.size() > 0)
			{
				out << YAML::Key << "ScriptFields" << YAML::Value;
				auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
				out << YAML::BeginSeq;
				for (const auto& [name, field] : fields)
				{
					if (entityFields.find(name) == entityFields.end())
						continue;

					out << YAML::BeginMap; // ScriptField
					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);

					out << YAML::Key << "Data" << YAML::Value;
					ScriptFieldInstance& scriptField = entityFields.at(name);

					switch (field.Type)
					{
						WRITE_SCRIPT_FIELD(Float, float);
						WRITE_SCRIPT_FIELD(Double, double);
						WRITE_SCRIPT_FIELD(Bool, bool);
						WRITE_SCRIPT_FIELD(Char, char);
						WRITE_SCRIPT_FIELD(Byte, int8_t);
						WRITE_SCRIPT_FIELD(Short, int16_t);
						WRITE_SCRIPT_FIELD(Int, int32_t);
						WRITE_SCRIPT_FIELD(Long, int64_t);
						WRITE_SCRIPT_FIELD(UByte, uint8_t);
						WRITE_SCRIPT_FIELD(UShort, uint16_t);
						WRITE_SCRIPT_FIELD(UInt, uint32_t);
						WRITE_SCRIPT_FIELD(ULong, uint64_t);
						//WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
						WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
						WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
						WRITE_SCRIPT_FIELD(Entity, uint32_t);
					}
					out << YAML::EndMap; // ScriptFields
				}
				out << YAML::EndSeq;
			}
#endif
			out << YAML::EndMap; // ScriptComponent
		}

		if (false && entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			//out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			//out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			//out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			//out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			//out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			//out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			//out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}
		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data = YAML::LoadFile(filepath);
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		Albedo_Core_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				Albedo_Core_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntity(name);

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					//auto& mc = deserializedEntity.GetComponent<MeshComponent>();
					//mc.Color = meshComponent["Color"].as<glm::vec4>();
					auto& mesh = meshComponent["Mesh"];
					std::string& name = mesh["Name"].as<std::string>();
					std::string& path = mesh["Path"].as<std::string>();
					deserializedEntity.AddComponent<MeshComponent>().AddMesh(m_AssetManager->LoadModel(path), (uint32_t)deserializedEntity);
				}

				auto textureComponent = entity["TextureComponent"];
				if (textureComponent)
				{
					auto& tc = deserializedEntity.AddComponent<TextureComponent>();

					auto& texture = textureComponent["Texture"];
					std::string& defaultTexture = texture["Default Texture"].as<std::string>();
					std::string& albedo = texture["Albedo"].as<std::string>();
					std::string& ambientOcclusion = texture["Ambient Occlusion"].as<std::string>();
					std::string& metallic = texture["Metallic"].as<std::string>();
					std::string& normal = texture["Normal"].as<std::string>();
					std::string& roughness = texture["Roughness"].as<std::string>();

					tc.AddTexture(m_AssetManager->LoadTexture(defaultTexture));
					tc.AddTexture(m_AssetManager->LoadTexture(albedo));
					tc.AddTexture(m_AssetManager->LoadTexture(ambientOcclusion));
					tc.AddTexture(m_AssetManager->LoadTexture(metallic));
					tc.AddTexture(m_AssetManager->LoadTexture(normal));
					tc.AddTexture(m_AssetManager->LoadTexture(roughness));

					tc.defaultTexture = textureComponent["isDefault"].as<bool>();
				}

				auto shaderComponent = entity["ShaderComponent"];
				if (shaderComponent)
				{
					auto& shader = shaderComponent["Shader"];
					std::string& path = shader["Path"].as<std::string>();
					deserializedEntity.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader(path));
				}

				auto materialComponent = entity["MaterialComponent"];
				if (materialComponent)
				{

				}

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.AddComponent<TransformComponent>();
					tc.Position = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto physicsComponent = entity["PhysicsComponent"];
				if (physicsComponent)
				{
					auto& tc = deserializedEntity.AddComponent<PhysicsComponent>();
					tc.BodyPosition = physicsComponent["Body Position"].as<glm::vec3>();
					tc.BodyOrientation = physicsComponent["Body Orientation"].as<glm::vec3>();
					tc.Force = physicsComponent["Force"].as<glm::vec3>();
					tc.Velocity = physicsComponent["Velocity"].as<glm::vec3>();
					tc.Mass = physicsComponent["Mass"].as<float>();
					tc.disableGravity = physicsComponent["Disable Gravity"].as<bool>();
					tc.restitution = physicsComponent["Restitution"].as<float>();
					tc.staticFriction = physicsComponent["Static Friction"].as<float>();
					tc.dynamicFriction = physicsComponent["Dynamic Friction"].as<float>();

					tc.bodyType = (PhysicsComponent::BodyType)physicsComponent["Body Type"].as<uint32_t>();
				}

				auto colliderComponent = entity["ColliderComponent"];
				if (colliderComponent)
				{
					auto& tc = deserializedEntity.AddComponent<ColliderComponent>();

					tc.ColliderPosition = colliderComponent["Collider Position"].as<glm::vec3>();
					tc.ColliderSize = colliderComponent["Collider Size"].as<glm::vec3>();
					tc.ColliderOrientation = colliderComponent["Collider Orientation"].as<glm::vec3>();
					tc.ColliderRadius = colliderComponent["Collider Radius"].as<float>();

					tc.colliderType = (ColliderComponent::ColliderType)colliderComponent["Collider Type"].as<uint32_t>();
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
#if 0
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
					sc.ClassName = scriptComponent["ClassName"].as<std::string>();

					auto scriptFields = scriptComponent["ScriptFields"];
					if (scriptFields)
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
						if (entityClass)
						{
							const auto& fields = entityClass->GetFields();
							auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

							for (auto scriptField : scriptFields)
							{
								std::string name = scriptField["Name"].as<std::string>();
								std::string typeString = scriptField["Type"].as<std::string>();
								ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

								ScriptFieldInstance& fieldInstance = entityFields[name];

								// TODO(Yan): turn this assert into Hazelnut log warning
								Albedo_CORE_ASSERT(fields.find(name) != fields.end(), "field not found");

								if (fields.find(name) == fields.end())
									continue;

								fieldInstance.Field = fields.at(name);

								switch (type)
								{
									READ_SCRIPT_FIELD(Float, float);
									READ_SCRIPT_FIELD(Double, double);
									READ_SCRIPT_FIELD(Bool, bool);
									READ_SCRIPT_FIELD(Char, char);
									READ_SCRIPT_FIELD(Byte, int8_t);
									READ_SCRIPT_FIELD(Short, int16_t);
									READ_SCRIPT_FIELD(Int, int32_t);
									READ_SCRIPT_FIELD(Long, int64_t);
									READ_SCRIPT_FIELD(UByte, uint8_t);
									READ_SCRIPT_FIELD(UShort, uint16_t);
									READ_SCRIPT_FIELD(UInt, uint32_t);
									READ_SCRIPT_FIELD(ULong, uint64_t);
									READ_SCRIPT_FIELD(Vector2, glm::vec2);
									READ_SCRIPT_FIELD(Vector3, glm::vec3);
									READ_SCRIPT_FIELD(Vector4, glm::vec4);
									READ_SCRIPT_FIELD(Entity, uint32_t);
							}
						}
					}
#endif
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
				}

				auto cameraComponent = entity["CameraComponent"];
#if 0
				if (cameraComponent)
				{
					//auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					//
					//auto& cameraProps = cameraComponent["Camera"];
					//cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
					//
					//cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					//cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					//cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
					//
					//cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					//cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					//cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					//cc.Primary = cameraComponent["Primary"].as<bool>();
					//cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}
#endif

				auto nativeScriptComponent = entity["NativeScriptComponent"];
				if (nativeScriptComponent)
				{

				}
			}
		}
			return true;
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		// Not implemented
		//Albedo_CORE_ASSERT(false);
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		// Not implemented
		//Albedo_CORE_ASSERT(false);
		return false;
	}

}