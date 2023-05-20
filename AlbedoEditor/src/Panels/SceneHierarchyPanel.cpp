#include "AlbedoPreCompiledHeader.h"

#include "SceneHierarchyPanel.h"	
#include "Albedo/Scene/Components.h"
#include "Albedo/Utils/AssetSystem.h"
#include "Albedo/Scripting/ScriptEngine.h"

#include <filesystem>

#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace Albedo {

	extern std::unique_ptr<Albedo::AssetSystem> m_AssetManager;

	SceneHierarchyPanel::SceneHierarchyPanel()
	{
		m_MeshIcon = Texture2D::Create("Assets/Textures/Wood.png", false);
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> context)
	{
		m_MeshIcon = Texture2D::Create("Assets/Textures/Wood.png", false);
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene> context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID , m_Context.get() };
				if (!default) 
				{
					m_SelectionContext = entity;
					default = true;
				}
				DrawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) //No entity selected
			m_SelectionContext = {};

		// Right-click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1))
		{
			if (ImGui::MenuItem("Create Default Entity"))
			{
				Entity e = m_Context->CreateEntity("Default Entity");

				e.AddComponent<MeshComponent>().AddMesh(m_AssetManager->LoadModel("Assets/models/suzanne/suzanne.obj"), (uint32_t)e);
				e.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Models/suzann/albedo.png"));
				e.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/ModelShader.glsl"));
				e.AddComponent<PhysicsComponent>();
				glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
				glm::vec3 size = glm::vec3(1.0, 1.0f, 1.0f);
				glm::vec3 rot = glm::vec3(0.0, 0.0f, 0.0f);
				e.GetComponent<MeshComponent>().m_Mesh->GetRendererConfig().Type = DrawType::Albedo_TRIANGLES;
				e.GetComponent<TransformComponent>().Position = pos;
				e.GetComponent<TransformComponent>().Scale = size;
				e.GetComponent<TransformComponent>().Rotation = rot;
				e.GetComponent<PhysicsComponent>();
				e.GetComponent<PhysicsComponent>().BodyPosition = pos;
				e.GetComponent<PhysicsComponent>().BodyOrientation = glm::quat(rot);
				e.GetComponent<PhysicsComponent>().bodyType = e.GetComponent<PhysicsComponent>().BodyType::Static;
				e.GetComponent<PhysicsComponent>().Mass = 0.f;
				e.AddComponent<ColliderComponent>();
				e.GetComponent<ColliderComponent>().ColliderPosition = pos;
				e.GetComponent<ColliderComponent>().ColliderSize = size;
				e.GetComponent<ColliderComponent>().ColliderOrientation = glm::quat(rot);
				e.GetComponent<ColliderComponent>().colliderType = e.GetComponent<ColliderComponent>().ColliderType::Box;

				m_SelectionContext = e;
			}
			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			int i = 0;
 			if (entity.HasComponent<TagComponent>())
			{
				auto e = entity.GetComponent<TagComponent>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)++i, flags, e.name.c_str()))
					ImGui::TreePop();
			}
			if (entity.HasComponent<MeshComponent>())
			{
				auto e = entity.GetComponent<MeshComponent>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)++i, flags, e.name.c_str()))
					ImGui::TreePop();
			}
			if (entity.HasComponent<TextureComponent>())
			{
				auto e = entity.GetComponent<TextureComponent>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)++i, flags, e.name.c_str()))
					ImGui::TreePop();
			}
			if (entity.HasComponent<ShaderComponent>())
			{
				auto e = entity.GetComponent<ShaderComponent>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)++i, flags, e.name.c_str()))
					ImGui::TreePop();
			}
			if (entity.HasComponent<MaterialComponent>())
			{
				auto e = entity.GetComponent<MaterialComponent>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)++i, flags, e.name.c_str()))
					ImGui::TreePop();
			}
			if (entity.HasComponent<PhysicsComponent>())
			{
				auto e = entity.GetComponent<PhysicsComponent>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)++i, flags, e.name.c_str()))
					ImGui::TreePop();
			}
			if (entity.HasComponent<SpriteRendererComponent>())
			{
				auto e = entity.GetComponent<SpriteRendererComponent>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)++i, flags, e.name.c_str()))
					ImGui::TreePop();
			}
			if (entity.HasComponent<CameraComponent>())
			{
				auto e = entity.GetComponent<CameraComponent>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)++i, flags, e.name.c_str()))
					ImGui::TreePop();
			}
			if (entity.HasComponent<ScriptComponent>())
			{
				auto e = entity.GetComponent<ScriptComponent>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)++i, flags, e.name.c_str()))
					ImGui::TreePop();
			}
			if (entity.HasComponent<NativeScriptComponent>())
			{
				auto e = entity.GetComponent<NativeScriptComponent>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)++i, flags, e.name.c_str()))
					ImGui::TreePop();
			}


			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(
			);
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			char* Tag = "Default Tag";
			ImGuiInputTextFlags input_text_flags =
				ImGuiInputTextFlags_EnterReturnsTrue   | 
				ImGuiInputTextFlags_EscapeClearsAll    | 
				ImGuiInputTextFlags_CallbackCompletion | 
				ImGuiInputTextFlags_CallbackHistory;

			if (ImGui::MenuItem("Tag"))
			{
				if (!m_SelectionContext.HasComponent<TagComponent>())
					m_SelectionContext.AddComponent<TagComponent>(Tag);
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::MenuItem("Mesh"))
			{
				if (!m_SelectionContext.HasComponent<MeshComponent>())
					m_SelectionContext.AddComponent<MeshComponent>();
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::MenuItem("Texture"))
			{
				if (!m_SelectionContext.HasComponent<TextureComponent>())
					m_SelectionContext.AddComponent<TextureComponent>();
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::MenuItem("Shader"))
			{
				if (!m_SelectionContext.HasComponent<ShaderComponent>())
					m_SelectionContext.AddComponent<ShaderComponent>();
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::MenuItem("Material"))
			{
				if (!m_SelectionContext.HasComponent<MaterialComponent>())
					m_SelectionContext.AddComponent<MaterialComponent>();
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::MenuItem("Transform"))
			{
				if (!m_SelectionContext.HasComponent<TransformComponent>())
					m_SelectionContext.AddComponent<TransformComponent>();
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::MenuItem("Physics"))
			{
				if (!m_SelectionContext.HasComponent<PhysicsComponent>())
					m_SelectionContext.AddComponent<PhysicsComponent>();
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::MenuItem("Script"))
			{
				if (!m_SelectionContext.HasComponent<ScriptComponent>())
					m_SelectionContext.AddComponent<ScriptComponent>();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Position, 0, 70);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation, 0, 70);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f, 70);
			});

		DrawComponent<MeshComponent>("Mesh", entity, [&](auto& component)
			{
				ImGui::Image(reinterpret_cast<void*>(m_MeshIcon->GetTextureID()), ImVec2{ 30.0f, 30.0f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				//ImGui::SameLine(65.0f, 1.0f);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path meshPath = std::filesystem::path(std::filesystem::path("Assets") / path);
						component.AddMesh(m_AssetManager->LoadModel(meshPath.string()), (uint32_t)entity);
					}
					ImGui::EndDragDropTarget();
				}
				if (component.m_Mesh)
				{
					const std::string& s = "Vertices: " + std::to_string(component.m_Mesh->GetTotalVertices());
					ImGui::Text(s.c_str());
				}
				const std::string& s = "Name: " + component.m_Mesh->GetPath();
				ImGui::Text(s.c_str());
			});

		DrawComponent<TextureComponent>("Texture", entity, [&](auto& component)
			{
				std::string items[] = {"Albedo", "Ambient Occlusion", "Metallic", "Normal", "Roughness"};
				if (ImGui::BeginCombo("##textures", m_CurrentShader.c_str()))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (m_CurrentShader == items[n]);
						if (ImGui::Selectable(items[n].c_str(), is_selected))
						{
							m_CurrentShader = items[n];
							switch (n)
							{
							case 0: // Albedo
								component.type = component.TextureType::Albedo;
								break;
							case 1: // Ambient Occlusion
								component.type = component.TextureType::AmbientOcclusion;
								break;
							case 2: // Metallic
								component.type = component.TextureType::Metallic;
								break;
							case 3: // Normal
								component.type = component.TextureType::Normal;
								break;
							case 4: // Roughness
								component.type = component.TextureType::Roughness;
								break;
							default:
								break;
							}
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				ImGui::Image(reinterpret_cast<void*>(m_MeshIcon->GetTextureID()), ImVec2{ 30.0f, 30.0f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(std::filesystem::path("Assets") / path);
						component.AddTexture(m_AssetManager->LoadTexture(texturePath.u8string()));
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::Separator();

				ImGui::Checkbox("Albedo", &m_Albedo);
				ImGui::SameLine();
				ImGui::Button("ao.tga");

				ImGui::Checkbox("Ambient Occlusion", &m_AmbientOcclusion);
				ImGui::SameLine();
				ImGui::Button("ao.tga");

				ImGui::Checkbox("Metallic", &m_Metallic);
				ImGui::SameLine();
				ImGui::Button("ao.tga");

				ImGui::Checkbox("Normal", &m_Normal);
				ImGui::SameLine();
				ImGui::Button("ao.tga");

				ImGui::Checkbox("Roughness", &m_Roughness);
				ImGui::SameLine();
				ImGui::Button("ao.tga");
			});

		DrawComponent<ShaderComponent>("Shader", entity, [&](auto& component)
			{
				std::string items[] = { "Vertex", "Fragment", "Compute", "Geometry"};
				if (ImGui::BeginCombo("##shaders", m_CurrentTexture.c_str()))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (m_CurrentTexture == items[n]);
						if (ImGui::Selectable(items[n].c_str(), is_selected))
							m_CurrentTexture = items[n];
						if(is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				ImGui::Image(reinterpret_cast<void*>(m_MeshIcon->GetTextureID()), ImVec2{ 30.0f, 30.0f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path meshPath = std::filesystem::path(std::filesystem::path("Assets") / path);
						//component.AddMesh(m_AssetManager->LoadModel(meshPath.string()), (uint32_t)entity);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::Separator();
				ImGui::Text("Current Shaders");
				ImGui::Button(component.m_Shader->GetPath().c_str(), ImVec2{60, 15});
			});

		DrawComponent<MaterialComponent>("Material", entity, [&](auto& component)
			{
				ImGui::Text("Irradiance");
				ImGui::Text("Roughness");
				ImGui::Text("Reflecion");
				ImGui::Text("Specular");
			});

		DrawComponent<PhysicsComponent>("Physics", entity, [&](auto& component)
			{
				ImGui::Checkbox("Enable Physics: ", &m_PhysicsEnabled);

				ImGui::Text("Body Type");
				ImGui::SameLine();
				std::string items[] = { "Static", "Dynamic" };
				if (ImGui::BeginCombo("##physics", m_CurrentPhysicsType.c_str()))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (m_CurrentPhysicsType == items[n]);
						if (ImGui::Selectable(items[n].c_str(), is_selected))
						{
							component.physicsMaterial = std::make_shared<PhysicsMaterial>
								(component.staticFriction, component.dynamicFriction, component.restitution);

							m_CurrentPhysicsType = items[n];
							m_CurrentShader = items[n];
							switch (n)
							{
							case 0: // Static
								component.staticBody = std::make_shared<RigidBodyStaticComponent>
									(component.BodyPosition, component.BodyOrientation);
								component.bodyType = component.BodyType::Static;
								break;
							case 1: // Dynamic
								component.dynamicBody = std::make_shared<RigidBodyDynamicComponent>
									(component.BodyPosition, component.BodyOrientation, component.Mass);
								component.bodyType = component.BodyType::Dynamic;
								break;
							default:
								break;
							}
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::Separator();
			});

		DrawComponent<ColliderComponent>("Collider", entity, [&](auto& component)
			{
				ImGui::Text("Collider Type");
				ImGui::SameLine();
				std::string items[] = { "Box", "Sphere", "Mesh", "ConvexMesh" };
				if (ImGui::BeginCombo("##collider", m_CurrentColliderType.c_str()))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (m_CurrentColliderType == items[n]);
						if (ImGui::Selectable(items[n].c_str(), is_selected))
						{
							auto& e = entity.GetComponent<PhysicsComponent>();
							m_CurrentColliderType = items[n];
							m_CurrentShader = items[n];
							switch (n)
							{
							case 0: // Box
							{
								if (e.bodyType == e.BodyType::Static && e.staticBody)
								{
									component.collider = std::make_shared<BoxCollider>
										(e.staticBody.get(), component.ColliderSize,
											e.physicsMaterial, component.ColliderPosition, component.ColliderOrientation);
								}
								else if (e.bodyType == e.BodyType::Dynamic && e.dynamicBody)
								{
									component.collider = std::make_shared<BoxCollider>
										(e.dynamicBody.get(), component.ColliderSize,
											e.physicsMaterial, component.ColliderPosition, component.ColliderOrientation);
								}
								component.colliderType = component.ColliderType::Box;
								break;
							}
							case 1: // Sphere
								component.colliderType = component.ColliderType::Sphere;
								break;
							case 2: // Mesh
								component.colliderType = component.ColliderType::Mesh;
								break;
							case 3: // Convex Mesh
								component.colliderType = component.ColliderType::ConvexMesh;
								break;
							default:
								break;
							}
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::Separator();
				//DrawVec3Control("Rotation", g, 0, 70);
				DrawVec3Control("Scale", component.ColliderSize, 1.0f, 70);
			});

		DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_Context](auto& component) mutable
			{
				bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

				static char buffer[64];
				strcpy_s(buffer, sizeof(buffer), component.ClassName.c_str());

				// Fields
				bool sceneRunning = scene->IsRunning();
				if (sceneRunning)
				{
					Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance((uint32_t)entity.GetEntityHandle());
					if (scriptInstance)
					{
						const auto& fields = scriptInstance->GetScriptClass()->GetFields();
						for (const auto& [name, field] : fields)
						{
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptInstance->GetFieldValue<float>(name);
								if (ImGui::DragFloat(name.c_str(), &data))
								{
									scriptInstance->SetFieldValue(name, data);
								}
							}
						}
					}
				}
				else
				{
					if (scriptClassExists)
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
						const auto& fields = entityClass->GetFields();

						auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
						for (const auto& [name, field] : fields)
						{
							// Field has been set in editor
							if (entityFields.find(name) != entityFields.end())
							{
								ScriptFieldInstance& scriptField = entityFields.at(name);

								// Display control to set it maybe
								if (field.Type == ScriptFieldType::Float)
								{
									float data = scriptField.GetValue<float>();
									if (ImGui::DragFloat(name.c_str(), &data))
										scriptField.SetValue(data);
								}
							}
							else
							{
								// Display control to set it maybe
								if (field.Type == ScriptFieldType::Float)
								{
									float data = 0.0f;
									if (ImGui::DragFloat(name.c_str(), &data))
									{
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue(data);
									}
								}
							}
						}
					}
				}
				if (!scriptClassExists)
					ImGui::PopStyleColor();
			});
	}

}