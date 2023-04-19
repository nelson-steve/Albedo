#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Albedo/Scene/SceneCamera.h"
#include"Albedo/Cameras/OrthographicCamera.h"
#include "ScriptableEntity.h"
#include "Albedo/Renderer/Mesh.h"

#include "Albedo/Physics/Collider.h"
#include "Albedo/Physics/BoxCollider.h"
#include "Albedo/Physics/SphererCollider.h"

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
		void AddMesh(const Ref<Mesh> mesh, int id) { m_Mesh = mesh; mesh->SetInitializationStatus(true); ID = id; }

		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Mesh> m_Mesh;
		int ID = -1;
		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
	};

	struct RigidBodyComponent
	{

	};

	struct TextureComponent
	{
		void AddTexture(const Ref<Texture2D>  texture) { m_Textures.push_back(texture); }

		std::vector<Ref<Texture2D>> m_Textures;

		TextureComponent() = default;
		TextureComponent(const TextureComponent&) = default;
	};

	struct ShaderComponent
	{
		void AddShader(const Ref<Shader> shader) { m_Shader = shader; }

		std::string m_Path = std::string();
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
		void AddTranform(const glm::mat4 tranform) { Transform = tranform; }
		const glm::vec3& GetPosition() const { return Position; }
		const glm::vec3& GetRotation() const { return Rotation; }
		const glm::vec3& GetScale() const { return Scale; }

		void AddTranform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale) 
		{ 
			glm::mat4 rotation = glm::toMat4(glm::quat(rot));
			Transform =  glm::translate(glm::mat4(1.0f), pos) * rotation * glm::scale(glm::mat4(1.0f), scale);
			Position = pos;
			Rotation = rot;
			Scale = scale;
		}

		glm::mat4 Transform{ 1.0f };
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale	   = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;


		TransformComponent(const glm::vec3& position)
			: Position(position) {}

		//void SetPosition(const glm::mat4& pos) { Position = pos; }

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct PhysicsComponent
	{
		glm::vec3 Position = glm::vec3(0.0);
		glm::vec3 Velocity = glm::vec3(0.0, 10.0, 0.0);
		glm::vec3 Force = glm::vec3(1.0);
		float Mass = 1;
		bool physicsEnabled = false;

		PhysicsComponent() = default;
		PhysicsComponent(const PhysicsComponent&) = default;
	};

	struct ColliderComponent
	{
		bool operator ==(ColliderComponent& c)
		{
			if (c.collider == this->collider) return true;
			else return false;
		}

		Ref<Collider> collider;

		bool ShowCollider;


		ColliderComponent() = default;
		ColliderComponent(const ColliderComponent&) = default;
	};

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

}