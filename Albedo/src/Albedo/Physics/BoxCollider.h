#pragma once

#include "Collider.h"

namespace Albedo {

	class BoxCollider : public Collider
	{
	public:
		virtual bool operator==(Collider& c) override
		{
			auto* sphere = dynamic_cast<BoxCollider*>(&c);
			if(
				this->GetCenter() == sphere->GetCenter() &&
				this->GetHeight() == sphere->GetHeight() &&
				this->GetWidth()  == sphere->GetWidth()  &&
				this->GetMin()	  == sphere->GetMin()    &&
				this->GetMax()    == sphere->GetMax()
			) return true;
			else return false;

			Albedo_Core_ERROR("Error: SphereColliders unknown");
			return false;
		}

		void SetWidth(float width) { m_Width = width; }
		void SetHeight(float height) { m_Height = height; }
		void SetDepth(float depth) { m_Depth = depth; }
		void SetCenter(const glm::vec3& center) { m_Center = center; }
		void SetMin(const glm::vec3& min) { m_Min = min; }
		void SetMax(const glm::vec3& max) { m_Max = max; }
		virtual void SetType(const Type& type) override { m_Type = type; }

		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }
		float GetDepth() const { return m_Depth; }
		const glm::vec3& GetCenter() const { return m_Center; }
		const glm::vec3& GetMin() const { return m_Min; }
		const glm::vec3& GetMax() const { return m_Max; }
		virtual const Type GetType() const override { return m_Type; }
	private:
		glm::vec3 m_Min	   = glm::vec3(0.0);
		glm::vec3 m_Max	   = glm::vec3(1.0);
		float m_Width	   = 1.0f;
		float m_Height	   = 1.0f;
		float m_Depth	   = 1.0f;
		glm::vec3 m_Center = glm::vec3(0.0);
	};

}