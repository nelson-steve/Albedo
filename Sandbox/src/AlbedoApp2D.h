#pragma once

#include "Albedo.h"

class AlbedoApp2D : public Albedo::Layer
{
public:
	AlbedoApp2D();
	~AlbedoApp2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Albedo::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Albedo::Event& e) override;
private:
	Albedo::Ref<Albedo::VertexArray> m_VertexArray;
	Albedo::Ref<Albedo::Shader> m_Shader;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec4 m_SquareColor = { 0.2, 0.4, 0.8, 1.0f };

	Albedo::OrthographicCameraController m_CameraController;
};