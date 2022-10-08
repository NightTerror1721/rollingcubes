#pragma once

#pragma warning(push)
#pragma warning(disable: 26819)

//#include <SDL.h>
//#include <GL/glew.h>
//#include <SDL_opengl.h>
//#include <GL/GLU.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#pragma warning(pop)

#include <unordered_map>
#include <vector>
#include <memory>

#include "utils/logger.h"


namespace gl
{
	bool initGLFW();
	bool initGLEW();
	void terminate();
}


namespace gl
{
	enum class DataType : GLenum
	{
		Byte = GL_BYTE,
		UnsignedByte = GL_UNSIGNED_BYTE,
		Short = GL_SHORT,
		UnsignedShort = GL_UNSIGNED_SHORT,
		Int = GL_INT,
		UnsignedInt = GL_UNSIGNED_INT,

		HalfFloat = GL_HALF_FLOAT,
		Float = GL_FLOAT,
		Double = GL_DOUBLE,
		Fixed = GL_FIXED,
		Int_2_10_10_10_rev = GL_INT_2_10_10_10_REV,
		UnsignedInt_2_10_10_10_rev = GL_UNSIGNED_INT_2_10_10_10_REV,
		UnsignedInt_10f_11f_11f_rev = GL_UNSIGNED_INT_10F_11F_11F_REV
	};

	enum class BufferType : GLenum
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

	enum class BufferUsage : GLenum
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
}


namespace gl
{
	namespace
	{
		class GLBuffer
		{
		public:
			using Id = unsigned int;

		private:
			Id _id = 0;
			GLsizeiptr _size = 0;
			BufferType _target;

		public:
			GLBuffer(const GLBuffer&) = delete;
			GLBuffer& operator= (const GLBuffer&) = delete;

			constexpr explicit GLBuffer(BufferType target) : _target(target) {}

			constexpr GLBuffer(GLBuffer&& right) noexcept : _id(right._id), _size(right._size), _target(right._target)
			{
				right._id = 0;
				right._size = 0;
			}

			constexpr GLBuffer& operator= (GLBuffer&& right) noexcept
			{
				std::destroy_at(this);
				return *std::construct_at<GLBuffer, GLBuffer&&>(this, std::move(right));
			}

			inline ~GLBuffer() { destroy(); }

		public:
			constexpr bool invalid() const { return _id == 0; }
			constexpr Id id() const { return _id; }
			constexpr bool empty() const { return _size == 0; }
			constexpr std::size_t size() const { return static_cast<std::size_t>(_size); }
			constexpr BufferType target() const { return _target; }

			inline bool create()
			{
				if (invalid())
					glGenBuffers(1, &_id);

				return _id != 0;
			}

			inline void bind() const { glBindBuffer(static_cast<GLenum>(_target), _id); }
			inline void unbind() const { glBindBuffer(static_cast<GLenum>(_target), 0); }

			inline bool set(const void* data, GLsizeiptr size, BufferUsage usage, bool createNew = true, bool unbindOnEnd = true)
			{
				if ((!createNew && invalid()) || (createNew && !create()))
					return false;

				bind();
				glBufferData(static_cast<GLenum>(_target), size, data, static_cast<GLenum>(usage));
				_size = size;
				if(unbindOnEnd)
					unbind();
				return true;
			}

			inline void destroy()
			{
				if (!invalid())
					glDeleteBuffers(1, &_id);

				_id = 0;
				_size = 0;
			}
		};
	}

	class VBO : public GLBuffer
	{
	public:
		VBO(const VBO&) = delete;
		VBO& operator= (const VBO&) = delete;

		constexpr VBO(VBO&& right) noexcept : GLBuffer(std::move(right)) {}

		constexpr VBO& operator= (VBO&& right) noexcept { return GLBuffer::operator=(std::move(right)), *this; }

		constexpr VBO() : GLBuffer(BufferType::Array) {}

		inline ~VBO() = default;
	};

	class EBO : public GLBuffer
	{
	public:
		EBO(const EBO&) = delete;
		EBO& operator= (const EBO&) = delete;

		constexpr EBO(EBO&& right) noexcept : GLBuffer(std::move(right)) {}

		constexpr EBO& operator= (EBO&& right) noexcept { return GLBuffer::operator=(std::move(right)), * this; }

		constexpr EBO() : GLBuffer(BufferType::ElementArray) {}

		inline ~EBO() = default;
	};

	class VAO
	{
	public:
		using Id = GLuint;

		class Attribute
		{
		public:
			using Id = unsigned int;

			friend VAO;

		private:
			GLint _size = 0;
			DataType _type = DataType::Byte;
			GLsizei _stride = 0;
			GLboolean _normalized = GL_FALSE;
			bool _enabled = false;
			std::unique_ptr<VBO> _vbo = nullptr;

		public:
			Attribute() = default;
			Attribute(const Attribute&) = delete;
			Attribute(Attribute&&) noexcept = delete;
			~Attribute() = default;

			Attribute& operator= (const Attribute&) = delete;
			Attribute& operator= (Attribute&&) noexcept = delete;

			constexpr GLint size() const { return _size; }
			constexpr DataType type() const { return _type; }
			constexpr GLsizei stride() const { return _stride; }
			constexpr GLboolean normalized() const { return _normalized; }
			constexpr bool enabled() const { return _enabled; }
			constexpr const VBO& vbo() const { return *_vbo; }

			constexpr bool useVBO() const { return _vbo != nullptr; }

			constexpr std::size_t elementsCount() const
			{
				if (_vbo == nullptr || _size == 0)
					return 0;
				return _vbo->size() / std::size_t(_size);
			}

		private:
			void clear()
			{
				_size = 0;
				_type = DataType::Byte;
				_stride = 0;
				_normalized = GL_FALSE;
				_enabled = false;
				_vbo.reset();
			}
		};

	private:
		Id _id = 0;
		std::unordered_map<Attribute::Id, Attribute> _attrib;
		std::unique_ptr<EBO> _ebo = nullptr;
		std::size_t _indicesCount = 0;

	public:
		VAO() = default;
		VAO(const VAO&) = delete;
		VAO& operator= (const VAO&) = delete;

		VAO(VAO&&) noexcept = default;
		VAO& operator= (VAO&&) noexcept = default;

		inline ~VAO() { destroy(); }

	public:
		inline bool valid() const { return _id != 0; }
		inline bool invalid() const { return _id == 0; }
		inline Id id() const { return _id; }
		inline const std::unordered_map<Attribute::Id, Attribute>& attributes() const { return _attrib; }
		inline const EBO& ebo() const { return *_ebo; }
		inline bool hasEBO() const { return _ebo != nullptr; }

		inline const Attribute& getAttribute(Attribute::Id attrId) const
		{
			return _attrib.at(attrId);
		}

		inline bool create()
		{
			if (invalid())
				glGenVertexArrays(1, &_id);

			return _id != 0;
		}

		inline void bind() const { glBindVertexArray(_id); }
		inline void unbind() const { glBindVertexArray(0); }

		inline void enableAttribute(Attribute::Id attrId)
		{
			if (!invalid())
			{
				bind();
				glEnableVertexAttribArray(attrId);
				_attrib[attrId]._enabled = true;
				unbind();
			}
		}

		inline void disableAttribute(Attribute::Id attrId)
		{
			if (!invalid())
			{
				bind();
				glDisableVertexAttribArray(attrId);
				_attrib[attrId]._enabled = false;
				unbind();
			}
		}

		inline bool setAttribute(
			Attribute::Id attrId,
			DataType dataType,
			GLint dataTypeSize,
			GLboolean normalized,
			GLsizei stride,
			const void* data,
			GLsizeiptr dataSize,
			BufferUsage usage,
			bool enableOnCreate = true,
			bool createNew = true)
		{
			if ((!createNew && invalid()) || (createNew && !create()))
				return false;

			bind();
			Attribute& attr = _attrib[attrId];
			attr.clear();
			attr._type = dataType;
			attr._size = dataTypeSize;
			attr._normalized = normalized;
			attr._stride = stride;
			attr._vbo = std::make_unique<VBO>();
			attr._vbo->set(data, dataSize, usage, true, false);
			glVertexAttribPointer(attrId, dataTypeSize, GLenum(dataType), normalized, stride, (void*)0);
			if (enableOnCreate)
			{
				glEnableVertexAttribArray(attrId);
				attr._enabled = true;
			}
			else attr._enabled = false;
			attr._vbo->unbind();
			unbind();

			return true;
		}

		inline bool setEBO(const void* data, GLsizeiptr dataSize, BufferUsage usage, bool createNew = true)
		{
			if ((!createNew && invalid()) || (createNew && !create()))
				return false;

			bind();
			_ebo.reset(new EBO());
			_ebo->set(data, dataSize, usage, true);
			_indicesCount = std::size_t(dataSize) / sizeof(GLuint);
			unbind();

			return true;
		}

		inline void destroy()
		{
			_attrib.clear();
			_ebo.reset();
			_indicesCount = 0;

			if (!invalid())
				glDeleteVertexArrays(1, &_id);
		}

		inline void render(Attribute::Id vertexAttributeId, GLenum mode = GL_TRIANGLES) const
		{
			if (!invalid())
			{
				bind();

				if (_indicesCount > 0 && _ebo != nullptr)
				{
					_ebo->bind();
					glDrawElements(mode, GLsizei(_indicesCount), GL_UNSIGNED_INT, nullptr);
					_ebo->unbind();
				}
				else
				{
					const auto& it = _attrib.find(vertexAttributeId);
					if (it != _attrib.end() && it->second.elementsCount() > 0)
					{
						glDrawArrays(mode, 0, GLsizei(it->second.elementsCount()));
					}
				}

				unbind();
			}
		}
	};
}
