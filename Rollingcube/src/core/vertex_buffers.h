#pragma once

#include "gl.h"

#include <utility>
#include <memory>

#include <vector>

#include "utils/raw_buffer.h"

namespace gl
{
	enum class VertexBufferType : GLenum
	{
		Array = GL_ARRAY_BUFFER,
		AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
		CopyRead = GL_COPY_READ_BUFFER,
		CopyWrite = GL_COPY_WRITE_BUFFER,
		DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,
		DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
		ElementArray = GL_ELEMENT_ARRAY_BUFFER,
		PixelPack = GL_PIXEL_PACK_BUFFER,
		PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
		Query = GL_QUERY_BUFFER,
		ShaderStorage = GL_SHADER_STORAGE_BUFFER,
		Texture = GL_TEXTURE_BUFFER,
		TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
		Uniform = GL_UNIFORM_BUFFER
	};

	enum class VertexBufferUsage : GLenum
	{
		StreamDraw = GL_STREAM_DRAW,
		StreamRead = GL_STREAM_READ,
		StreamCopy = GL_STREAM_COPY,
		StaticDraw = GL_STATIC_DRAW,
		StaticRead = GL_STATIC_READ,
		StaticCopy = GL_STATIC_COPY,
		DynamicDraw = GL_DYNAMIC_DRAW,
		DynamicRead = GL_DYNAMIC_READ,
		DynamicCopy = GL_DYNAMIC_COPY
	};

	template <VertexBufferType _Type>
	class VertexBufferObject;

	using VBO = VertexBufferObject<VertexBufferType::Array>;
	using EBO = VertexBufferObject<VertexBufferType::ElementArray>;



	template <VertexBufferType _Type>
	class VertexBufferObject
	{
	public:
		using Id = GLuint;
		using Type = VertexBufferType;
		using Usage = VertexBufferUsage;
		using SizeType = GLsizeiptr;

	private:
		Id _id = 0;
		SizeType _size = 0;
		Usage _usage = Usage(0);
		GLsizei _elementCount = 0;

	public:
		constexpr VertexBufferObject() noexcept = default;

		VertexBufferObject(const VertexBufferObject&) = delete;
		VertexBufferObject& operator= (const VertexBufferObject&) = delete;

		constexpr VertexBufferObject(VertexBufferObject&& right) noexcept :
			_id(right._id), _size(right._size), _usage(right._usage), _elementCount(right._elementCount)
		{
			right._id = 0;
			right._size = 0;
			right._usage = Usage(0);
			right._elementCount = 0;
		}

		constexpr VertexBufferObject& operator= (VertexBufferObject&& right) noexcept
		{
			std::destroy_at(this);
			return *std::construct_at<VertexBufferObject, VertexBufferObject&&>(this, std::move(right));
		}

		inline ~VertexBufferObject() { destroy(); }

	public:
		constexpr bool isCreated() const { return _id != 0; }
		constexpr Id getId() const { return _id; }
		constexpr bool empty() const { return _size == 0; }
		constexpr SizeType size() const { return _size; }
		constexpr Usage getUsage() const { return _usage; }
		constexpr GLsizei getElementCount() const { return _elementCount; }
		static constexpr Type type() { return _Type; }

		constexpr operator bool() const { return _id != 0; }
		constexpr bool operator! () const { return _id == 0; }

		inline bool create()
		{
			if (!isCreated())
				glGenBuffers(1, &_id);

			return isCreated();
		}

		inline void destroy()
		{
			if (isCreated())
				glDeleteBuffers(1, &_id);

			_id = 0;
			_size = 0;
		}

		inline void bind() const { glBindBuffer(static_cast<GLenum>(_Type), _id); }
		inline void unbind() const { glBindBuffer(static_cast<GLenum>(_Type), 0); }

		inline bool write(const void* data, std::size_t dataTypeSize, std::size_t count, Usage usage, bool createIfNot = true, bool unbindOnEnd = true)
		{
			if ((!createIfNot && !isCreated()) || (createIfNot && !create()))
				return false;

			bind();
			glBufferData(static_cast<GLenum>(_Type), SizeType(dataTypeSize * count), data, static_cast<GLenum>(usage));
			_size = SizeType(dataTypeSize * count);
			_elementCount = GLsizei(count);
			if (unbindOnEnd)
				unbind();
			return true;
		}

		template <typename _Ty>
		inline bool write(const _Ty* data, std::size_t count, Usage usage, bool createIfNot = true, bool unbindOnEnd = true)
		{
			return write(data, sizeof(_Ty), count, usage, createIfNot, unbindOnEnd);
		}

		template <typename _Ty>
		inline bool write(const std::vector<_Ty>& data, Usage usage, bool createIfNot = true, bool unbindOnEnd = true)
		{
			return write(&data[0], sizeof(_Ty), data.size(), usage, createIfNot, unbindOnEnd);
		}

		inline bool write(const RawBuffer& data, Usage usage, bool createIfNot = true, bool unbindOnEnd = true)
		{
			return write(data.data(), 1, data.size(), usage, createIfNot, unbindOnEnd);
		}
	};
}
