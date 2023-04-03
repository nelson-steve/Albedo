#include "AlbedoPreCompiledHeader.h"

#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"

#include <fstream>

#include <yaml-cpp/yaml.h>
#include <Albedo/Core/Application.cpp>


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

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		:m_Scene(scene) 
	{

	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << "12837192831273"; // TODO: Entity ID goes here

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

			auto& texture = tc.m_Textures;
			out << YAML::Key << "Texture" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "Path" << YAML::Value << texture[0]->GetPath();
			out << YAML::EndMap;

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

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

			out << YAML::EndMap; // SpriteRendererComponent
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
					deserializedEntity.AddComponent<MeshComponent>().AddMesh(m_AssetManager->LoadModel(path));
				}

				auto textureComponent = entity["TextureComponent"];
				if (textureComponent)
				{
					auto& texture = textureComponent["Texture"];
					std::string& path = texture["Path"].as<std::string>();
					deserializedEntity.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture(path));
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
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Position = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
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