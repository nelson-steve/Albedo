#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Albedo/Scene/SceneCamera.h"
#include"Albedo/Cameras/OrthographicCamera.h"
#include "ScriptableEntity.h"
#include "Albedo/Renderer/Model.h"

namespace Albedo {

	struct TagComponent
	{
		std::string name = "Tag Component";
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct MaterialComponent
	{
		std::string name = "Material Component";

		//Ref<GLTF_Material> m_Material;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
	};

	struct ModelComponent
	{
		std::string name = "Model Component";

		void AddMesh(const Ref<Model> model, int id) { m_Model = model; ID = id; }

		Ref<Model> m_Model;
		int ID = -1;

		ModelComponent() = default;
		ModelComponent(const ModelComponent&) = default;
	};

	struct LightComponent
	{
		enum LightType
		{
			Point,
			Directional,
			Spot
		};
		
		LightType type		= LightType::Directional;
		
		glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
		glm::vec3 position  = glm::vec3(1.0);
		glm::vec3 ambient   = glm::vec3(0.05f, 0.05f, 0.05f);
		glm::vec3 diffuse   = glm::vec3(0.4f, 0.4f, 0.4f);
		glm::vec3 specular  = glm::vec3(0.5f, 0.5f, 0.5f);

		float cutOff        = 0.0;
		float outerCutOff	= 0.0;
		float constant		= 0.0;
		float linear		= 0.0;
		float quadratic		= 0.0;

		std::string nameOfLight = "Directional";
		//glm::vec3 color = glm::vec3(1.0);

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
	};

	struct SkyboxComponent
	{
		Ref<Texture2D> m_Skybox;
		Ref<Model> m_Model;

		std::string path = "Assets/Textures/hdr/environment.hdr";

		SkyboxComponent() = default;
		SkyboxComponent(const SkyboxComponent&) = default;
	};

	struct TextureComponent
	{
		std::string name = "Texture Compnent";

		static enum TextureType
		{
			Albedo = 0,
			AmbientOcclusion, 
			Metallic,
			Normal,
			Roughness
		};

		void AddTexture(const Ref<Texture2D> texture, int i)
		{
			type = static_cast<TextureType>(i);
			m_Textures[type] = texture;
			m_TextureNames[type] = texture->GetName();
		}
		TextureType type = TextureType::Albedo;
		uint32_t totalTypes = 5;
		std::unordered_map<TextureType, Ref<Texture2D>> m_Textures;
		std::unordered_map<TextureType, std::string> m_TextureNames;
		bool initialize = true;

		TextureComponent() = default;
		TextureComponent(const TextureComponent&) = default;
	};

	struct ShaderComponent
	{
		std::string name = "Shader Compnent";

		void AddShader(const Ref<Shader> shader) 
		{ 
			m_Shader = shader; 
			m_Shader->SetInitializationStatus(true);
		}

		Ref<Shader> m_Shader;
		bool tobeinitialized = false;

		ShaderComponent() = default;
		ShaderComponent(const ShaderComponent&) = default;
	};

	struct TransformComponent
	{
		std::string name = "Transform Compnent";

		void AddTranform(const glm::mat4 tranform) { Transform = tranform; }
		const glm::vec3& GetPosition() const { return Position; }
		const glm::vec3& GetRotation() const { return Rotation; }
		const glm::vec3& GetScale() const { return Scale; }

		void AddTranform(const glm::vec3& pos)
		{
			Transform = glm::translate(glm::mat4(1.0f), pos);
			Position = pos;
		}

		void AddTranform(const glm::vec4& rot)
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(rot));
			Transform = Transform * rotation;
		}

		void AddTranform(const glm::vec3& pos, const glm::vec4& rot)
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(rot));
			Transform = glm::translate(glm::mat4(1.0f), pos) * rotation;
			Position = pos;
			Rotation = rot;
		}

		void AddTranform(const glm::vec3& pos, const glm::vec4& rot, const glm::vec3& scale) 
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

	// Physics

	struct Physics2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;
		bool Gravity = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Physics2DComponent() = default;
		Physics2DComponent(const Physics2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct PhysicsComponent
	{
		std::string name = "Physics Compnent";

		enum BodyType
		{
			Static = 0,
			Dynamic
		};

		float staticFriction = 0.05;
		float dynamicFriction = 0.005;
		float restitution = 0.0;

		// Rigid Body
		glm::vec3 BodyPosition = glm::vec3(0.0);
		glm::quat BodyOrientation = glm::quat(1.0, 0.0, 0.0, 0.0);
		BodyType bodyType = BodyType::Dynamic;
		glm::vec3 Force = glm::vec3(0.0);
		glm::vec3 Velocity = glm::vec3(0.0, 0.0, 0.0);
		float Mass = 1;
		//bool infiniteMass = false;

		//states
		bool disableGravity = true;
		bool isKinematic = false;
		float SphereColliderRadius = 1.0f;

		std::string phyTypeName = "Dynamic";

		Ref<PhysicsMaterial> physicsMaterial;
		Ref<RigidBodyDynamicComponent> dynamicBody;	
		Ref<RigidBodyStaticComponent> staticBody;
		bool initialize = true;

		PhysicsComponent() = default;
		PhysicsComponent(const PhysicsComponent&) = default;
	};

	struct ColliderComponent
	{
		enum ColliderType
		{
			Box = 0,
			Sphere,
			Mesh,
			ConvexMesh
		};

		ColliderType colliderType = ColliderType::Box;
		glm::vec3 ColliderPosition = glm::vec3(0.0);
		float ColliderRadius = 1;
		glm::vec3 ColliderSize = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::quat ColliderOrientation = glm::quat(1.0, 0.0, 0.0, 0.0);

		std::string colTypeName = "Box";
		

		Ref<PhysicsCollider> collider;
		bool initialize = true;
		
		ColliderComponent() = default;
		ColliderComponent(const ColliderComponent&) = default;
	};

	struct SpriteRendererComponent
	{
		std::string name = "Sprite Renderer Compnent";

		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CameraComponent
	{
		std::string name = "Camera Compnent";

		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string name = "Script Compnent";

		std::string ClassName = "Sandbox.Player";

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	struct NativeScriptComponent
	{
		std::string name = "Native Script Compnent";

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

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<ModelComponent, TransformComponent, LightComponent, SkyboxComponent, Physics2DComponent, BoxCollider2DComponent,
		PhysicsComponent, ColliderComponent, TagComponent, ShaderComponent, TextureComponent, MaterialComponent, 
		ScriptComponent>;

}