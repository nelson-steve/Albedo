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
		m_BlackTexture = Texture2D::Create("Assets/Textures/DarkGrey.jpeg", false);
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> context)
	{
		m_MeshIcon = Texture2D::Create("Assets/Textures/Wood.png", false);
		m_BlackTexture = Texture2D::Create("Assets/Textures/DarkGrey.jpeg", false);
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
			if (ImGui::MenuItem("Create Default Model"))
			{
				Entity e = m_Context->CreateMeshEntity("Default Model");

				m_SelectionContext = e;
			}

			if (ImGui::MenuItem("Create Default Cube"))
			{
				Entity e = m_Context->CreateCubeEntity("Default Cube");

				m_SelectionContext = e;
			}

			if (ImGui::MenuItem("Create Default Plane"))
			{
				Entity e = m_Context->CreatePlaneEntity("Default Plane");
			
				m_SelectionContext = e;
			}

			if (ImGui::MenuItem("Create Scene Light"))
			{
				Entity e = m_Context->CreateLightEntity("Scene Light");

				m_SelectionContext = e;
			}

			if (ImGui::MenuItem("Create Skybox"))
			{
				Entity e = m_Context->CreateSkyboxEntity("Skybox");

				m_SelectionContext = e;
			}

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Entities");
		if (m_SelectionContext)
		{
			DrawComponentsOfEntity(m_SelectionContext);
		}

		ImGui::End();

		ImGui::Begin("Scene Enties");
		if (m_SelectionContext)
		{
			DrawComponentsOfScene(m_SelectionContext);
		}

		ImGui::End();

		ImGui::Begin("Scene Setting");
		{
			auto& sceneSetting = m_Context->GetSceneSetting();
			ImGui::Checkbox("Show Collider", &sceneSetting.ShowCollider);
			ImGui::DragFloat("Background Light", &sceneSetting.backgroundLight, 0.01f, 0.0, 1.0f);
			ImGui::Checkbox("Enable Gravity", &sceneSetting.enableGravity);
		}

		ImGui::End();
	}
#if 0
	if (entity.HasComponent<LightComponent>())
	{
		auto& view = m_Context->m_Registry.view<LightComponent, TagComponent>();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)234234, flags, "Scene Lights");
		int i = 0;
		if (opened)
		{
			auto& tag = entity.GetComponent<TagComponent>();
			ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)i, flags, tag.Tag.c_str());
			if (ImGui::IsItemClicked)
			{
				m_SelectionContext = e;
			}
			if (o)
				ImGui::TreePop();

			ImGui::TreePop();
		}

		return;
	}
#endif

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.c_str());
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
			if (entity.HasComponent<ModelComponent>())
			{
				auto e = entity.GetComponent<ModelComponent>();
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
			ImGui::PopStyleVar();
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

	void SceneHierarchyPanel::DrawComponentsOfEntity(Entity entity)
	{
		if (entity.HasComponent<LightComponent>() || entity.HasComponent<SkyboxComponent>())
			return;

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

			if (!m_SelectionContext.HasComponent<TagComponent>())
			{
				if (ImGui::MenuItem("Tag"))
				{
					if (!m_SelectionContext.HasComponent<TagComponent>())
						m_SelectionContext.AddComponent<TagComponent>(Tag);
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<ModelComponent>())
			{
				if (ImGui::MenuItem("Mesh"))
				{
					if (!m_SelectionContext.HasComponent<ModelComponent>())
						m_SelectionContext.AddComponent<ModelComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			//if (!m_SelectionContext.HasComponent<LightComponent>())
			//{
			//	if (ImGui::MenuItem("Light"))
			//	{
			//		if (!m_SelectionContext.HasComponent<LightComponent>())
			//			m_SelectionContext.AddComponent<LightComponent>();
			//		ImGui::CloseCurrentPopup();
			//	}
			//}
			if (!m_SelectionContext.HasComponent<TextureComponent>())
			{
				if (ImGui::MenuItem("Texture"))
				{
					if (!m_SelectionContext.HasComponent<TextureComponent>())
						m_SelectionContext.AddComponent<TextureComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<ShaderComponent>())
			{
				if (ImGui::MenuItem("Shader"))
				{
					if (!m_SelectionContext.HasComponent<ShaderComponent>())
						m_SelectionContext.AddComponent<ShaderComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<MaterialComponent>())
			{
				if (ImGui::MenuItem("Material"))
				{
					if (!m_SelectionContext.HasComponent<MaterialComponent>())
						m_SelectionContext.AddComponent<MaterialComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<TransformComponent>())
			{
				if (ImGui::MenuItem("Transform"))
				{
					if (!m_SelectionContext.HasComponent<TransformComponent>())
						m_SelectionContext.AddComponent<TransformComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<Physics2DComponent>())
			{
				if (ImGui::MenuItem("RigidBody2D"))
				{
					if (!m_SelectionContext.HasComponent<Physics2DComponent>())
						m_SelectionContext.AddComponent<Physics2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::MenuItem("BoxCollider"))
				{
					if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>())
						m_SelectionContext.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
#if 0
			else if (ImGui::MenuItem("Physics"))
			{
				if (!m_SelectionContext.HasComponent<PhysicsComponent>())
					m_SelectionContext.AddComponent<PhysicsComponent>();
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::MenuItem("Collider"))
			{
				if (!m_SelectionContext.HasComponent<ColliderComponent>())
					m_SelectionContext.AddComponent<ColliderComponent>();
				ImGui::CloseCurrentPopup();
			}
#endif

			if (!m_SelectionContext.HasComponent<ScriptComponent>())
			{
				if (ImGui::MenuItem("Script"))
				{
					if (!m_SelectionContext.HasComponent<ScriptComponent>())
						m_SelectionContext.AddComponent<ScriptComponent>();
					ImGui::CloseCurrentPopup();
				}
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

		DrawComponent<ModelComponent>("Model", entity, [&](auto& component)
			{
				ImGui::Image(reinterpret_cast<void*>(m_BlackTexture->GetTextureID()), ImVec2{ 30.0f, 30.0f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 }, ImVec4{ 1, 1, 1, 1 }, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
				//ImGui::SameLine(65.0f, 1.0f);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path meshPath = std::filesystem::path(std::filesystem::path("Assets") / path);
						//component.initialize = true; // TODO: See if this needs to be here or not
						//if (m_AssetManager->ValidateModelPath(meshPath.string()))
						//	component.AddMesh(m_AssetManager->LoadModel(meshPath.string()), (uint32_t)entity);
					}
					ImGui::EndDragDropTarget();
				}
				//if (component.m_Model)
				//{
				//	const std::string& s = "Vertices " + std::to_string(component.m_Mesh->GetTotalVertices());
				//	ImGui::Text(s.c_str());
				//}
				//const std::string& s = "Name " + component.m_Mesh->GetName();
				//ImGui::Button(s.c_str());
			});

		DrawComponent<TextureComponent>("Texture", entity, [&](auto& component)
			{
				std::string items[] = {"Albedo", "Ambient Occlusion", "Metallic", "Normal", "Roughness"};
				if (ImGui::BeginCombo("##textures", m_CurrentTexture.c_str()))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (m_CurrentTexture == items[n]);
						if (ImGui::Selectable(items[n].c_str(), is_selected))
						{
							m_CurrentTexture = items[n];
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
				ImGui::Image(reinterpret_cast<void*>(m_BlackTexture->GetTextureID()), ImVec2{ 30.0f, 30.0f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 }, ImVec4{ 1, 1, 1, 1 }, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(std::filesystem::path("Assets") / path);
						//component.initialize = true; // TODO: See if this needs to be here or not
						if (m_AssetManager->ValidateTexturePath(texturePath.string()))
							component.AddTexture(m_AssetManager->LoadTexture(texturePath.u8string()), (int)component.type);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::Separator();
				if (entity.GetComponent<MaterialComponent>().isPBR)
				{
					ImGui::Checkbox("Albedo", &m_Albedo);
					ImGui::SameLine();
					ImGui::Button(component.m_Textures[component.TextureType::Albedo]->GetName().c_str());

					ImGui::Checkbox("Ambient Occlusion", &m_AmbientOcclusion);
					ImGui::SameLine();
					ImGui::Button(component.m_Textures[component.TextureType::AmbientOcclusion]->GetName().c_str());

					ImGui::Checkbox("Metallic", &m_Metallic);
					ImGui::SameLine();
					ImGui::Button(component.m_Textures[component.TextureType::Metallic]->GetName().c_str());

					ImGui::Checkbox("Normal", &m_Normal);
					ImGui::SameLine();
					ImGui::Button(component.m_Textures[component.TextureType::Normal]->GetName().c_str());

					ImGui::Checkbox("Roughness", &m_Roughness);
					ImGui::SameLine();
					ImGui::Button(component.m_Textures[component.TextureType::Roughness]->GetName().c_str());
				}
				else
				{
					ImGui::Checkbox("Albedo", &m_Albedo);
					ImGui::SameLine();
					ImGui::Button(component.m_Textures[component.TextureType::Albedo]->GetName().c_str());
				}
			});

		DrawComponent<ShaderComponent>("Shader", entity, [&](auto& component)
			{
				std::string items[] = { "Vertex", "Fragment", "Compute", "Geometry"};
				if (ImGui::BeginCombo("##shaders", m_CurrentShader.c_str()))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (m_CurrentShader == items[n]);
						if (ImGui::Selectable(items[n].c_str(), is_selected))
							m_CurrentShader = items[n];
						if(is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				ImGui::Image(reinterpret_cast<void*>(m_BlackTexture->GetTextureID()), ImVec2{ 30.0f, 30.0f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 }, ImVec4{ 1, 1, 1, 1 }, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path shaderPath = std::filesystem::path(std::filesystem::path("Assets") / path);
						//component.initialize = true; // TODO: See if this needs to be here or not
						if (m_AssetManager->ValidateShaderPath(shaderPath.string()))
							component.AddShader(m_AssetManager->LoadShader(shaderPath.u8string()));
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::Separator();

				const std::string& s = "Name " + component.m_Shader->GetName();
				ImGui::Button(s.c_str());
			});

		DrawComponent<MaterialComponent>("Material", entity, [&](auto& component)
			{
				ImGui::Checkbox("PBR", &component.isPBR);

				component.m_Material->SetPBRStatus(component.isPBR);

				ImGui::DragFloat("Shininess", &component.shininess, 0.1f, 0.001f, 40.0f);
				component.m_Material->SetShininess(component.shininess);
				ImGui::DragFloat("Exposure", &exposure, 0.1f, 0.0f, 10.0f);
				component.m_Material->SetExposure(exposure);
				ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.0f, 5.0f);
				component.m_Material->SetRoughnessScale(roughness);
			});

		DrawComponent<Physics2DComponent>("Rigidbody 2D", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Physics2DComponent::BodyType)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
			});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size	));
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});
#if 0
		DrawComponent<PhysicsComponent>("Physics", entity, [&](auto& component)
			{
				if (ImGui::Checkbox("Disable Gravity", &component.disableGravity))
					component.initialize = true;
				//if (ImGui::Checkbox("Infinite Mass", &component.infiniteMass))
				//	component.initialize = true;
				if (ImGui::Checkbox("Kinematic", &component.isKinematic))
					component.initialize = true;

				ImGui::Separator();
				ImGui::Separator();
				ImGui::Separator();

				if (ImGui::DragFloat("Mass", &component.Mass))
					component.initialize = true;

				ImGui::Separator();
				
				if (ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 1.0f))
					component.initialize = true;
				if (ImGui::DragFloat("Dynamic Friction", &component.dynamicFriction, 1.0f, 0.0f))
					component.initialize = true;
				if (ImGui::DragFloat("Static Friction", &component.staticFriction, 1.0f, 0.0f))
					component.initialize = true;

				ImGui::Separator();

				ImGui::Text("Body Type");
				ImGui::SameLine();
				std::string items[] = { "Static", "Dynamic" };
				if (ImGui::BeginCombo("##physics", items[component.bodyType].c_str()))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (component.phyTypeName == items[n]);
						if (ImGui::Selectable(items[n].c_str(), is_selected))
						{
							//component.physicsMaterial = std::make_shared<PhysicsMaterial>
							//	(component.staticFriction, component.dynamicFriction, component.restitution);

							component.phyTypeName = items[n];
							switch (n)
							{
							case 0: // Static
								component.bodyType = component.BodyType::Static;
								component.initialize = true;
								break;
							case 1: // Dynamic
								component.bodyType = component.BodyType::Dynamic;
								component.initialize = true;
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
				if (ImGui::BeginCombo("##collider", component.colTypeName.c_str()))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (component.colTypeName == items[n]);
						if (ImGui::Selectable(items[n].c_str(), is_selected))
						{
							auto& e = entity.GetComponent<PhysicsComponent>();
							component.colTypeName = items[n];
							switch (n)
							{
								case 0: // Box
								{
									component.colliderType = component.ColliderType::Box;
									component.initialize = true;
									break;
								}
								case 1: // Sphere
									component.colliderType = component.ColliderType::Sphere;
									component.initialize = true;
									break;
								case 2: // Mesh
									component.colliderType = component.ColliderType::Mesh;
									component.initialize = true;
									break;
								case 3: // Convex Mesh
									component.colliderType = component.ColliderType::ConvexMesh;
									component.initialize = true;
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
#endif
#if 1
		DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_Context](auto& component) mutable
			{
				if (ImGui::BeginCombo("##classes", component.ClassName.c_str()))
				{
					int i = 0;
					for (const auto& EntityClass : ScriptEngine::GetEntityClasses())
					{
						bool is_selected = (component.ClassName == EntityClass.first);
						if (ImGui::Selectable(EntityClass.first.c_str(), is_selected))
						{
							component.ClassName = EntityClass.first;
							switch (i)
							{
								case 0:
								{
									break;
								}
							}
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
						i++;
					}
					ImGui::EndCombo();
				}
				ImGui::Separator();

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
#endif
	}

	void SceneHierarchyPanel::DrawComponentsOfScene(Entity entity)
	{
		DrawComponent<LightComponent>("Light", entity, [&](auto& component)
			{
				ImGui::Text("Light Component");

				if (component.type == component.LightType::Point)
				{
					DrawVec3Control("Position", component.position, 0, 70);
					ImGui::ColorEdit3("Ambient", glm::value_ptr(component.ambient));
					ImGui::ColorEdit3("Diffuse", glm::value_ptr(component.diffuse));
					ImGui::ColorEdit3("Specular", glm::value_ptr(component.specular));
					ImGui::DragFloat("Constant", &component.constant, 0.01, 0.0, 1.0);
					ImGui::DragFloat("Linear", &component.linear, 0.001, 0.0, 1.0);
					ImGui::DragFloat("Quadratic", &component.quadratic, 0.001, 0.0, 2.0);
				}
				else if (component.type == component.LightType::Directional)
				{
					DrawVec3Control("Direction", component.direction, 0, 70);
					ImGui::ColorEdit3("Ambient", glm::value_ptr(component.ambient));
					ImGui::ColorEdit3("Diffuse", glm::value_ptr(component.diffuse));
					ImGui::ColorEdit3("Specular", glm::value_ptr(component.specular));
				}
				else if (component.type == component.LightType::Spot)
				{
					DrawVec3Control("Position", component.position, 0, 70);
					DrawVec3Control("Direction", component.direction, 0, 70);
					ImGui::ColorEdit3("Ambient", glm::value_ptr(component.ambient));
					ImGui::ColorEdit3("Diffuse", glm::value_ptr(component.diffuse));
					ImGui::ColorEdit3("Specular", glm::value_ptr(component.specular));
					ImGui::DragFloat("Constant", &component.constant, 0.01, 0.0, 1.0);
					ImGui::DragFloat("Linear", &component.linear, 0.001, 0.0, 1.0);
					ImGui::DragFloat("Quadratic", &component.quadratic, 0.001, 0.0, 2.0);
					ImGui::DragFloat("CutOff", &component.cutOff, 1.0, 0.0, 100.0);
					ImGui::DragFloat("OuterCutOff", &component.outerCutOff, 1.0, 0.0, 100.0);
				}

				std::string items[] = { "Point", "Directional", "Spot" };
				if (ImGui::BeginCombo("##lights", component.nameOfLight.c_str()))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (component.nameOfLight == items[n]);
						if (ImGui::Selectable(items[n].c_str(), is_selected))
						{
							component.nameOfLight = items[n];
							switch (n)
							{
							case 0: // Point
								component.type = component.LightType::Point;
								break;
							case 1: // Directional
								component.type = component.LightType::Directional;
								break;
							case 2: // Spot
								component.type = component.LightType::Spot;
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

				//ImGui::ColorButton("color", ImVec4(color.x, color.y, color.z, color.w), 0, ImVec2());
				//DrawVec3Control("", rotation, 0, 70);
				//component.Rotation = glm::radians(rotation);
				//DrawVec3Control("Scale", component.Scale, 1.0f, 70);
			});

		DrawComponent<SkyboxComponent>("Skybox", entity, [&](auto& component)
			{
				ImGui::Text("Skybox Component");

				component.faces.resize(6);
				std::vector<std::string> sideNames = 
				{
					"Right",
					"Left",
					"Top",
					"Bottom",
					"Front",
					"Back"
				};
				for (int i = 0; i < 6; i++)
				{
					ImGui::Image(reinterpret_cast<void*>(m_BlackTexture->GetTextureID()), ImVec2{ 30.0f, 30.0f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 }, ImVec4{ 1, 1, 1, 1 }, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = std::filesystem::path(std::filesystem::path("Assets") / path);
							if (m_AssetManager->ValidateTexturePath(texturePath.string()))
								component.faces[i] = texturePath.string();
						}
						ImGui::EndDragDropTarget();
					}
					ImGui::SameLine();
					ImGui::Text(sideNames[i].c_str());
					if (!component.faces[i].empty())
					{
						ImGui::SameLine();
						ImGui::Text(component.faces[i].c_str());
					}
				}

				//for (const auto& face : component.faces)
				//{
				//	if (face.empty())
				//		continue;
				//	ImGui::Text(face.c_str());
				//}
				if (ImGui::Button("Reload Skybox"))
				{
					TextureConfiguration config(Config::TextureType::Cubemap, Config::InternalFormat::RGB, Config::TextureLayout::ClampToEdge,
						Config::MinMagFilters::LINEAR, Config::MinMagFilters::LINEAR, Config::DataType::UNSIGNED_BYTE,
						Config::DataFormat::RGB, false, false);
					config.Faces = component.faces;
					component.m_Skybox = Texture2D::Create(config);
				}

			});

	}

}