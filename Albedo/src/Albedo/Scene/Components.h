#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Albedo/Scene/SceneCamera.h"
#include "ScriptableEntity.h"

namespace Albedo {

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct MeshComponent
	{
		void AddMesh(const Ref<Mesh> mesh) { m_Mesh = mesh; }

		Ref<Mesh> m_Mesh;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
	};

	struct TextureComponent
	{
		void AddTexture(const Ref<Texture>  texture) { m_Textures.push_back(texture); }

		std::vector<Ref<Texture>>  m_Textures;

		TextureComponent() = default;
		TextureComponent(const TextureComponent&) = default;
	};

	struct ShaderComponent
	{
		void AddShader(const Ref<Shader> shader) { m_Shader = shader; }

		Ref<Shader> m_Shader;

		ShaderComponent() = default;
		ShaderComponent(const ShaderComponent&) = default;
	};

	struct MaterialComponent
	{
		void AddMaterial(const Ref<Material> material) { m_Material = material; }

		Ref<Material> m_Material;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
	};

	struct TransformComponent
	{
		glm::mat4 Transform{ 1.0f };
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale	   = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Position(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

#if 1
	//struct TagComponent
	//{
	//	std::string Tag;
	//
	//	TagComponent() = default;
	//	TagComponent(const TagComponent&) = default;
	//	TagComponent(const std::string& tag)
	//		: Tag(tag) {}
	//};

	//struct TransformComponent
	//{
	//	glm::mat4 Transform{ 1.0f };
	//	glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
	//	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	//	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
	//
	//	TransformComponent() = default;
	//	TransformComponent(const TransformComponent&) = default;
	//	TransformComponent(const glm::vec3& translation)
	//		: Translation(translation) {}
	//
	//	glm::mat4 GetTransform() const
	//	{
	//		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
	//		return glm::translate(glm::mat4(1.0f), Translation)
	//			* rotation
	//			* glm::scale(glm::mat4(1.0f), Scale);
	//	}
	//};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};
#endif

}