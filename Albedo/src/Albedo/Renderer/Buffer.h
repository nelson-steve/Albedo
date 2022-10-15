#pragma once

#include "Albedo/Core.h"
#include <vector>

namespace Albedo {
	enum class ShaderDataType
	{
		None = 0,
		Bool,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4
	};

	static unsigned int ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Bool:		return		   1;
		case ShaderDataType::Mat3:		return 4 * 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4 * 4;
		case ShaderDataType::Int:		return		   4;
		case ShaderDataType::Int2:		return	   4 * 2;
		case ShaderDataType::Int3:		return	   4 * 3;
		case ShaderDataType::Int4:		return	   4 * 4;
		case ShaderDataType::Float:		return		   1;
		case ShaderDataType::Float2:	return	   4 * 2;
		case ShaderDataType::Float3:	return	   4 * 3;
		case ShaderDataType::Float4:	return	   4 * 4;
		}
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		unsigned int Size;
		unsigned int Offset;
		bool Normalized;

		BufferElement(){}
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			:Name(name),Type(type),Size(ShaderDataTypeSize(type)),Offset(0),Normalized(normalized) {}
		//~BufferElement()

		unsigned int GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Bool:		return 1;
			case ShaderDataType::Mat3:		return 3;
			case ShaderDataType::Mat4:		return 4;
			case ShaderDataType::Int:		return 1;
			case ShaderDataType::Int2:		return 2;
			case ShaderDataType::Int3:		return 3;
			case ShaderDataType::Int4:		return 4;
			case ShaderDataType::Float:		return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;
			}
			Albedo_Core_INFO("Unknown Data Type");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			:m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		inline const unsigned int GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const{ return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetAndStride() {
			unsigned int offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		unsigned int m_Stride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static VertexBuffer* Create(float* vertices, unsigned int size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual unsigned int GetCount() const = 0;

		static IndexBuffer* Create(unsigned int* indices, unsigned int size);
	};
}