#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Albedo/Scene/SceneSerializer.h"
#include "Albedo/Utils/PlatformUtils.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Albedo/Math/Math.h"
#include "Albedo/Renderer/RendererConfig.h"
#include "Albedo/Scripting/ScriptEngine.h"

#include "ImGuizmo.h"


namespace Albedo {

	extern const std::filesystem::path g_AssetPath;
	extern std::unique_ptr<Albedo::AssetSystem> m_AssetManager;

	EditorLayer::EditorLayer()
		:Layer("Editor Layer"), m_CameraController(1280.0f / 720.0f)
	{

	}

	void EditorLayer::OnAttach()
	{
		m_ActiveScene = std::make_shared<Scene>();

		//Entity sceneCamera = m_ActiveScene->CreateEntity("SceneCamera");
		//sceneCamera.AddComponent<CameraComponent>();
		//sceneCamera.AddComponent<MeshComponent>().AddMesh(m_AssetManager->LoadModel("Assets/models/camera/light.obj"), (uint32_t)sceneCamera);
		//sceneCamera.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/models/fa/Diffuse.jpg", true));
		//sceneCamera.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/Shaders/ModelShader.glsl"));
		//sceneCamera.AddComponent<TransformComponent>().AddTranform(glm::vec3(0.0, 0.0, 0.0), glm::vec3(glm::radians(180.0), 0.0, 0.0), glm::vec3(0.1));
		//sceneCamera.GetComponent<MeshComponent>().m_Mesh->GetRendererConfig().Type = DrawType::Albedo_TRIANGLES;
		//sceneCamera.GetComponent<TransformComponent>().Position = glm::vec3(0.0, 0.0, 75.0);

#if 0
		{
			Entity suzanneMesh = m_ActiveScene->CreateEntity("Sphere");
			suzanneMesh.AddComponent<MeshComponent>().AddMesh(m_AssetManager->LoadModel("Assets/models/sphere/sphere_wrong_tangents.obj"), (uint32_t)suzanneMesh);
			suzanneMesh.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Models/suzanne/albedo.png"));
			suzanneMesh.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/Shaders/ModelShader.glsl"));
			suzanneMesh.AddComponent<PhysicsComponent>();
			glm::vec3 pos(0.0f, 400.0f, 0.0f);
			glm::vec3 size = glm::vec3(1.0, 1.0f, 1.0f);
			suzanneMesh.GetComponent<TransformComponent>().Position = pos;
			//suzanneMesh.GetComponent<TransformComponent>().Scale = size;
			suzanneMesh.GetComponent<MeshComponent>().m_Mesh->GetRendererConfig().Type = DrawType::Albedo_TRIANGLES;
			suzanneMesh.GetComponent<PhysicsComponent>().BodyPosition = pos;
			suzanneMesh.GetComponent<PhysicsComponent>().Mass = 1.0f;
			suzanneMesh.AddComponent<ColliderComponent>();
			//suzanneMesh.GetComponent<PhysicsComponent>().PhysicsEnabled = false;
		}
		{
			Entity suzanneMesh1 = m_ActiveScene->CreateEntity("Plane");
			suzanneMesh1.AddComponent<MeshComponent>().AddMesh(m_AssetManager->LoadModel("Assets/models/suzanne/suzanne.obj"), (uint32_t)suzanneMesh1);
			suzanneMesh1.AddComponent<TextureComponent>().AddTexture(m_AssetManager->LoadTexture("Assets/Models/concrete/dirty_concrete_metall_1k.png"));
			suzanneMesh1.AddComponent<ShaderComponent>().AddShader(m_AssetManager->LoadShader("Assets/Shaders/ModelShader.glsl"));
			suzanneMesh1.AddComponent<PhysicsComponent>();
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
			//glm::vec3 size = glm::vec3(300.0, 300.0f, 300.0f);
			glm::vec3 size = glm::vec3(50.0, 5.0f, 50.0f);
			glm::vec3 rot = glm::vec3(0.0, 0.0f, 10.0f);
			suzanneMesh1.GetComponent<MeshComponent>().m_Mesh->GetRendererConfig().Type = DrawType::Albedo_TRIANGLES;
			suzanneMesh1.GetComponent<TransformComponent>().Position = pos;
			//suzanneMesh1.GetComponent<TransformComponent>().Scale = size;
			suzanneMesh1.GetComponent<TransformComponent>().Rotation = rot;
			suzanneMesh1.GetComponent<PhysicsComponent>();
			suzanneMesh1.GetComponent<PhysicsComponent>().BodyPosition = pos;
			suzanneMesh1.GetComponent<PhysicsComponent>().BodyOrientation = glm::quat(rot);
			suzanneMesh1.GetComponent<PhysicsComponent>().Mass = 0.f;
			suzanneMesh1.AddComponent<ColliderComponent>();
			suzanneMesh1.GetComponent<ColliderComponent>().ColliderPosition = pos;
			suzanneMesh1.GetComponent<ColliderComponent>().ColliderSize = size;
		}
#endif
		m_IconSimulate = Texture2D::Create("Assets/Textures/UI/PlayButtonBlack.png", false);
		m_IconStop = Texture2D::Create("Assets/Textures/UI/PauseButtonBlack.png", false);

		m_IconPlay = Texture2D::Create("Assets/Textures/UI/PlayButtonBlack1.png", false);
		m_IconPause = Texture2D::Create("Assets/Textures/UI/PlayButtonBlack1.png", false);
		m_IconStep = Texture2D::Create("Assets/Textures/UI/PlayButtonBlack1.png", false);

		m_ActiveScene->InitScene();

		//auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		//if (commandLineArgs.Count > 1)
		//{
		//	auto sceneFilePath = commandLineArgs[1];
		//	OpenScene(sceneFilePath);
		//}

		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = {
				FramebufferTextureFormat::RGBA8,
				FramebufferTextureFormat::RED_INTEGER,
				FramebufferTextureFormat::DEPTH24STENCIL8
			};
			fbSpec.Width = 1280;
			fbSpec.Height = 720;
			m_Framebuffer = Framebuffer::Create(fbSpec);
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 10000.0f);

#if 0
		class CameraController : public ScriptableEntity
	
		public:
			virtual void OnCreate() override
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				translation.x = rand() % 10 - 5.0f;
			}

			virtual void OnDestroy() override
			{
			}

			virtual void OnUpdate(Timestep ts) override
			{
				auto& translation = GetComponent<TransformComponent>().Translation;

				float speed = 5.0f;

				if (Input::IsKeyPressed(KeyCode::A))
					translation.x -= speed * ts;
				if (Input::IsKeyPressed(KeyCode::D))
					translation.x += speed * ts;
				if (Input::IsKeyPressed(KeyCode::W))
					translation.y += speed * ts;
				if (Input::IsKeyPressed(KeyCode::S))
					translation.y -= speed * ts;
			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		//OpenDefaultScene("DefaultScene.albedo");
	}

	void EditorLayer::OnDetach()
	{
		Albedo_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		//Albedo_Core_INFO("Time: {}", ts.GetTime());
		
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		
		// Resize
		if (Albedo::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
		
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			//m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		m_Framebuffer->Bind();
		RenderCommand::ClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
		RenderCommand::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				//Albedo_Core_INFO("Scene: Edit Mode");
				if (m_ViewportFocused)
					m_CameraController.OnUpdate(ts);

				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateEditor(m_EditorCamera, ts);
				break;
			}
			case SceneState::Simulate:
			{
				//Albedo_Core_INFO("Scene: Edit Mode");
				if (m_ViewportFocused)
					m_CameraController.OnUpdate(ts);

				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateEditor(m_EditorCamera, ts);
				break;
			}
			case SceneState::Play:
			{
				//Albedo_Core_INFO("Scene: Play Mode");
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
			}
		}

		#if 0
		float value = 10.0f;
		for (float y = -value; y < value; y += 0.5f)
		{
			for (float x = -value; x < value; x += 0.5f)
			{
				glm::vec4 color = { (x + value) / 10.0f, 0.4f, (y + value) / 10.0f, 0.7f };
				Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		#endif // !1

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;
		
		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			//Albedo_Core_WARN("Pixel data = {0}", pixelData);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockingEnabled = true;
		if (dockingEnabled)
		{
			static bool dockspaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			bool opt_fullscreen = opt_fullscreen_persistant;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
			// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
			// all active windows docked into it will lose their parent and become undocked.
			// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
			// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// DockSpace
			ImGuiIO& io = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();
			float minWinSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 250.0f;
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

			style.WindowMinSize.x = minWinSizeX;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows, 
					// which we can't undo at the moment without finer window depth/z control.
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

					if (ImGui::MenuItem("New", "Ctrl+N")) NewScene();

					if (ImGui::MenuItem("Open...", "Ctrl+O")) OpenScene();

					if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) SaveSceneAs();

					if (ImGui::MenuItem("Exit"))
						Application::Get().Close();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Script"))
				{
					if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
						ScriptEngine::ReloadAssembly();
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			m_SceneHierarchyPanel.OnImGuiRender();
			m_ContentBrowserPanel.OnImGuiRender();

			ImGui::Begin("depth map");

			ImGui::Image((ImTextureID)m_ActiveScene->m_DepthMapFBO->GetDepthAttachmentID(), ImVec2{200, 200}, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			ImGui::End();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0);
			ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					OpenScene(std::filesystem::path(g_AssetPath) / path);
				}
				ImGui::EndDragDropTarget();
			}

			// Gizmos
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			if (selectedEntity && m_GizmoType != -1)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

				//Camera
				//auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
				//const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
				//const glm::mat4& cameraProjection = camera.GetProjection();
				//glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

				// Editor camera
				const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
				glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

				// Entity transform
				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 transform = tc.GetTransform();

				// Snapping
				bool snap = Input::IsKeyPressed(Key::LeftControl);
				float snapValue = 0.5f; // Snap to 0.5m for translation/scale
				// Snap to 45 degrees for rotation
				if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
					nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					Math::DecomposeTransform(transform, translation, rotation, scale);

					glm::vec3 deltaRotation = rotation - tc.Rotation;
					tc.Position= translation;
					tc.Rotation += deltaRotation;
					tc.Scale = scale;
				}
			}

			ImGui::End();
			ImGui::PopStyleVar();

			UI_Toolbar();

			ImGui::End();
		}
		else
		{
			Albedo_Core_ERROR("Docking disabled");
		}
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];	
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_ActiveScene;

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
			tintColor.w = 0.5f;

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		//bool hasPlayButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play;
		bool hasSimulateButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate;
		//bool hasPauseButton = m_SceneState != SceneState::Edit;

		//if (hasPlayButton)
		//{
		//	Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
		//	if (ImGui::ImageButton((ImTextureID)icon->GetTextureID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
		//	{
		//		if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
		//			OnScenePlay();
		//		else if (m_SceneState == SceneState::Play)
		//			OnSceneStop();
		//	}
		//}
		if (hasSimulateButton)
		{
			//if (hasPlayButton)
				ImGui::SameLine();

			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)icon->GetTextureID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}
		//if (hasPauseButton)
		//{
		//	bool isPaused = m_ActiveScene->IsPaused();
		//	ImGui::SameLine();
		//	{
		//		Ref<Texture2D> icon = m_IconPause;
		//		if (ImGui::ImageButton((ImTextureID)icon->GetTextureID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
		//		{
		//			m_ActiveScene->SetPaused(!isPaused);
		//		}
		//	}
		//
		//	// Step button
		//	if (isPaused)
		//	{
		//		ImGui::SameLine();
		//		{
		//			Ref<Texture2D> icon = m_IconStep;
		//			bool isPaused = m_ActiveScene->IsPaused();
		//			if (ImGui::ImageButton((ImTextureID)icon->GetTextureID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
		//			{
		//				m_ActiveScene->Step();
		//			}
		//		}
		//	}
		//}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);

		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (control)
					NewScene();

				break;
			}
			case Key::O:
			{
				if (control)
					OpenScene();

				break;
			}
			case Key::S:
			{
				if (control && shift)
					SaveSceneAs();

				break;
			}

			// Gizmos
			case Key::Q:
			{
				if (!ImGuizmo::IsUsing()) 
					m_GizmoType = -1;
				break;
			}
			case Key::W:
			{
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case Key::E:
			{
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case Key::R:
			{
				if (control)
				{
					ScriptEngine::ReloadAssembly();
				}
				else
				{
					if (!ImGuizmo::IsUsing())
						m_GizmoType = ImGuizmo::OPERATION::SCALE;
				}
				break;
			}
		}
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt) && m_HoveredEntity)
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::OpenDefaultScene(const std::string& path)
	{
		Ref<Scene> newScene = std::make_shared<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path))
		{
			m_EditorScene = newScene;
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
		}

		m_ActiveScene->InitScene();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = std::make_shared<Scene>();
		//m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::optional<std::string> filepath = FileDialogs::OpenFile("Albedo Scene (*.albedo)\0*.albedo\0");
		if (filepath)
		{
			OpenScene(filepath->c_str());
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		if (path.extension().string() != ".albedo")
		{
			Albedo_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = std::make_shared<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			//m_TempScene = m_ActiveScene;	
			m_EditorScene = newScene;
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_ActiveScene->InitScene();
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::optional<std::string> filepath = FileDialogs::SaveFile("Albedo Scene (*.albedo)\0*.albedo\0");
		
		if (filepath)
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(*filepath);
		}
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		//m_EditorScene = Scene::Copy(m_ActiveScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (!savedScenePath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(savedScenePath);
		}

		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_ActiveScene->InitScene();

		//m_EditorScene = Scene::Copy(m_ActiveScene);

		m_SceneState = SceneState::Simulate;

		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		Albedo_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate, "invalid scnene state");

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();

		Ref<Scene> newScene = std::make_shared<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(savedScenePath))
		{
			m_EditorScene = newScene;
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
		}

		//m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->InitScene();

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnScenePause()
	{
		if (m_SceneState == SceneState::Edit)
			return;

		m_ActiveScene->SetPaused(true);
	}

}

/*
#include <chrono>

template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func)
		: m_Name(name), m_Func(func), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;
		m_Func({ m_Name, duration });
	}
private:
	const char* m_Name;
	Fn m_Func;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })
*/