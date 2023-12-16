#pragma once

#include "Albedo/Cameras/Camera.h"

namespace Albedo {

	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetPerspective			   (float verticalFOV, float nearClip, float farClip);
		void SetViewportSize		   (uint32_t width, uint32_t height);
		void SetPerspectiveVerticalFOV (float verticalFov) { m_PerspectiveFOV  = verticalFov; RecalculateProjection(); }
		void SetPerspectiveNearClip    (float nearClip)    { m_PerspectiveNear = nearClip;    RecalculateProjection(); }
		void SetPerspectiveFarClip	   (float farClip)     { m_PerspectiveFar  = farClip;	  RecalculateProjection(); }
		void SetPosition(const glm::vec3& pos) { m_Position = pos; RecalculateView(); RecalculateProjection(); }

		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV;  }
		float GetPerspectiveNearClip()    const { return m_PerspectiveNear; }
		float GetPerspectiveFarClip()     const { return m_PerspectiveFar;  }

		virtual const glm::mat4& GetView() const override { return m_View; }
		virtual const glm::mat4& GetProjectionView() const override { return m_Projection * m_View; }
		virtual const glm::mat4& GetProjection() const override { return m_Projection; }
		virtual const glm::vec3& GetPosition() const override { return m_Position; }
	private:
		void RecalculateView();
		void RecalculateProjection();
	private:
		float m_PerspectiveFOV  = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f;
		float m_PerspectiveFar  = 1000.0f;
		float m_AspectRatio     = 0.0f;

		glm::vec3 m_Front    = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_Up		 = glm::vec3(0.0f, 1.0f, 0.0f);
		float offset = 5.0f;
	};

	class SceneOrthoCamera : public Camera
	{
	public:
		SceneOrthoCamera();
		virtual ~SceneOrthoCamera() = default;

		void SetOrthographic		 (float size, float nearClip, float farClip);
		void SetOrthographicSize	 (float size)	  { m_OrthographicSize = size;	    RecalculateProjection(); }
		void SetOrthographicNearClip (float nearClip) { m_OrthographicNear = nearClip;  RecalculateProjection(); }
		void SetOrthographicFarClip  (float farClip)  { m_OrthographicFar  = farClip;   RecalculateProjection(); }

		float GetOrthographicSize()     const { return m_OrthographicSize; }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		float GetOrthographicFarClip()  const { return m_OrthographicFar;  }
	private:
		void RecalculateProjection();
	private:
		float m_OrthographicSize =  10.0f;
		float m_OrthographicNear = -1.0f;
		float m_OrthographicFar  =  1.0f;
		float m_AspectRatio = 0.0f;
	};

}