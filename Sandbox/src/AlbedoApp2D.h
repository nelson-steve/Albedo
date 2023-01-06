#pragma once

#include "Albedo.h"

class AlbedoApp2D : public Albedo::Layer
{
public:
	AlbedoApp2D();
	virtual ~AlbedoApp2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Albedo::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Albedo::Event& e) override;
private:
	Albedo::OrthographicCameraController m_CameraController;

	Albedo::Ref<Albedo::VertexArray> m_VertexArray;
	Albedo::Ref<Albedo::Shader> m_Shader;
	Albedo::Ref<Albedo::Texture2D> m_Texture;
	Albedo::Ref<Albedo::Texture2D> m_Texture1;
	//struct ProfileResult
	//{
	//	const char* Name;
	//	float Time;
	//};

	//std::vector<ProfileResult> m_ProfileResults;

	glm::vec4 m_SquareColor = { 0.0f, 0.0f, 1.0f, 1.0f };

};