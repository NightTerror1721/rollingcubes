#pragma once

#include "vertex_buffers.h"

#include <unordered_map>


namespace gl
{
	class VertexArrayObject;
	using VAO = VertexArrayObject;


	enum class VertexArrayComponentCount : GLint
	{
		One = 1,
		Two = 2,
		Three = 3,
		Four = 4
	};
	static constexpr VertexArrayComponentCount to_component_count(GLint count) { return VertexArrayComponentCount(count); }


	class VertexArrayAttribute
	{
	public:
		using Id = GLuint;
		using ComponentCount = VertexArrayComponentCount;
		using SizeType = GLsizei;

		friend VertexArrayObject;

	private:
		ComponentCount _componentCount = ComponentCount(0);
		DataType _type = DataType(0);
		SizeType _stride = 0;
		GLboolean _normalized = GL_FALSE;
		bool _enabled = false;
		VBO _vbo = {};

	public:
		VertexArrayAttribute() = default;
		VertexArrayAttribute(const VertexArrayAttribute&) = delete;

		VertexArrayAttribute& operator= (const VertexArrayAttribute&) = delete;

		inline ~VertexArrayAttribute() { clear(); }

	public:
		constexpr bool isEnabled() const { return _enabled; }
		constexpr ComponentCount getComponentCount() const { return _componentCount; }
		constexpr DataType getDataType() const { return _type; }
		constexpr SizeType getStride() const { return _stride; }
		constexpr GLboolean isNormalized() const { return _normalized; }
		constexpr SizeType getElementCount() const { return _vbo.getElementCount(); }
		constexpr const VBO& getVertexBufferObject() const { return _vbo; }

		constexpr operator bool() const { return _enabled; }
		constexpr bool operator! () const { return !_enabled; }

	private:
		inline void clear()
		{
			_componentCount = ComponentCount(0);
			_type = DataType(0);
			_stride = 0;
			_normalized = GL_FALSE;
			_enabled = false;
			_vbo.destroy();
		}

		inline void set(ComponentCount componentCount, DataType dataType, SizeType stride, GLboolean normalized, VBO&& vbo)
		{
			_componentCount = componentCount;
			_type = dataType;
			_stride = stride;
			_normalized = normalized;
			_enabled = false;
			_vbo = std::move(vbo);
		}

		constexpr void enable() { _enabled = true; }
		constexpr void disable() { _enabled = false; }
	};


	class VertexArrayObject
	{
	public:
		using Id = GLuint;
		using Attribute = VertexArrayAttribute;
		using SizeType = GLsizei;

	private:
		Id _id = 0;
		std::unordered_map<Attribute::Id, Attribute> _attributes;
		Attribute::Id _verticesAttributeId = 0;

	public:
		VertexArrayObject() = default;
		VertexArrayObject(const VertexArrayObject&) = delete;
		VertexArrayObject(VertexArrayObject&&) noexcept = default;

		VertexArrayObject& operator= (const VertexArrayObject&) = delete;
		VertexArrayObject& operator= (VertexArrayObject&&) noexcept = default;

		inline ~VertexArrayObject() { destroy(); }

	public:
		constexpr bool isCreated() const { return _id != 0; }
		constexpr Id getId() const { return _id; }
		constexpr Attribute::Id getVerticesAttributeId() const { return _verticesAttributeId; }

		constexpr void setVerticesAttributeId(Attribute::Id id) { _verticesAttributeId = id; }
		
		constexpr operator bool() const { return _id != 0; }
		constexpr bool operator!() const { return _id == 0; }

		inline bool getAttributesCount() const { return _attributes.size(); }
		inline bool hasAttribute(Attribute::Id id) const { return _attributes.contains(id); }
		inline const Attribute& getAttribute(Attribute::Id id) const { return _attributes.at(id); }

		inline const Attribute& operator[] (Attribute::Id id) const { return _attributes.at(id); }

		inline bool create()
		{
			if (!isCreated())
				glGenVertexArrays(1, &_id);

			return isCreated();
		}

		inline void destroy()
		{
			if (isCreated())
				glDeleteVertexArrays(1, &_id);

			_id = 0;
			_attributes.clear();
		}

		inline void bind() const { glBindVertexArray(_id); }
		inline void unbind() const { glBindVertexArray(0); }

		inline void enableAttribute(Attribute::Id id)
		{
			if (isCreated() && hasAttribute(id))
			{
				bind();
				glEnableVertexAttribArray(id);
				_attributes.at(id).enable();
				unbind();
			}
		}

		inline void disableAttribute(Attribute::Id id)
		{
			if (isCreated() && hasAttribute(id))
			{
				bind();
				glDisableVertexAttribArray(id);
				_attributes.at(id).disable();
				unbind();
			}
		}

		inline bool createAttribute(
			Attribute::Id attributeId,
			Attribute::ComponentCount componentCount,
			DataType attributeDataType,
			GLboolean normalized,
			Attribute::SizeType stride,
			VBO&& vertexBufferObject,
			GLsizeiptr offset = 0,
			bool enableOnCreate = true,
			bool createIfNot = true

		) {
			if ((!createIfNot && !isCreated()) || (createIfNot && !create()) || !vertexBufferObject.isCreated())
				return false;

			bind();

			Attribute& attr = _attributes[attributeId];
			attr.set(componentCount, attributeDataType, stride, normalized, std::move(vertexBufferObject));

			attr._vbo.bind();
			glVertexAttribPointer(attributeId, GLint(componentCount), GLenum(attributeDataType), normalized, stride, reinterpret_cast<void*>(offset));
			if (enableOnCreate)
			{
				glEnableVertexAttribArray(attributeId);
				attr.enable();
			}
			attr._vbo.unbind();

			unbind();

			return true;
		}

		inline bool createAttribute(
			Attribute::Id attributeId,
			Attribute::ComponentCount componentCount,
			DataType attributeDataType,
			GLboolean normalized,
			Attribute::SizeType stride,
			const void* data,
			std::size_t dataTypeSize,
			std::size_t count,
			VBO::Usage usage,
			GLsizeiptr offset = 0,
			bool enableOnCreate = true,
			bool createIfNot = true
		) {
			VBO vbo;
			vbo.write(data, dataTypeSize, count, usage, true, true);
			return createAttribute(
				attributeId,
				componentCount,
				attributeDataType,
				normalized,
				stride,
				std::move(vbo),
				offset,
				enableOnCreate,
				createIfNot
			);
		}

		template <typename _Ty>
		inline bool createAttribute(
			Attribute::Id attributeId,
			Attribute::ComponentCount componentCount,
			DataType attributeDataType,
			GLboolean normalized,
			Attribute::SizeType stride,
			const _Ty* data,
			std::size_t count,
			VBO::Usage usage,
			GLsizeiptr offset = 0,
			bool enableOnCreate = true,
			bool createIfNot = true
		) {
			VBO vbo;
			vbo.write<_Ty>(data, count, usage, true, true);
			return createAttribute(
				attributeId,
				componentCount,
				attributeDataType,
				normalized,
				stride,
				std::move(vbo),
				offset,
				enableOnCreate,
				createIfNot
			);
		}

		template <typename _Ty>
		inline bool createAttribute(
			Attribute::Id attributeId,
			Attribute::ComponentCount componentCount,
			DataType attributeDataType,
			GLboolean normalized,
			Attribute::SizeType stride,
			const std::vector<_Ty>& data,
			VBO::Usage usage,
			GLsizeiptr offset = 0,
			bool enableOnCreate = true,
			bool createIfNot = true
		) {
			VBO vbo;
			vbo.write<_Ty>(data, usage, true, true);
			return createAttribute(
				attributeId,
				componentCount,
				attributeDataType,
				normalized,
				stride,
				std::move(vbo),
				offset,
				enableOnCreate,
				createIfNot
			);
		}

		inline bool createAttribute(
			Attribute::Id attributeId,
			Attribute::ComponentCount componentCount,
			DataType attributeDataType,
			GLboolean normalized,
			Attribute::SizeType stride,
			const RawBuffer& data,
			VBO::Usage usage,
			GLsizeiptr offset = 0,
			bool enableOnCreate = true,
			bool createIfNot = true
		) {
			VBO vbo;
			vbo.write(data, usage, true, true);
			return createAttribute(
				attributeId,
				componentCount,
				attributeDataType,
				normalized,
				stride,
				std::move(vbo),
				offset,
				enableOnCreate,
				createIfNot
			);
		}
	};
}
