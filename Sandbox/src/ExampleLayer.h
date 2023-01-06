#pragma once

#include "Albedo.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Albedo/Renderer/Texture.h"
#include "Albedo/Core/Core.h"
#include "Albedo/Cameras/OrthographicCameraController.h"

class ExampleLayer : public Albedo::Layer
{
public:
	ExampleLayer();

	void OnUpdate(Albedo::Timestep ts);
	virtual void OnImGuiRender() override;

	void OnEvent(Albedo::Event& e) override;
private:
	Albedo::Ref<Albedo::VertexArray> m_VertexArray;
	Albedo::Ref<Albedo::Shader> m_Shader;

	Albedo::Ref<Albedo::Texture2D> m_Texture;
	Albedo::Ref<Albedo::Texture2D> m_Texture2;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec4 m_BigColor = { 0.2, 0.4, 0.8, 1.0f };
	glm::vec4 m_SmallColor = { 0.5, 0.5, 0.9, 1.0f };

	Albedo::OrthographicCameraController m_CameraController;

};