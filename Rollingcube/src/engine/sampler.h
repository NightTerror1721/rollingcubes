#pragma once

#include "core/gl.h"
#include "utils/manager.h"


class Sampler
{
public:
	using Id = GLuint;
	using MagnificationFilter = gl::MagnificationFilter;
	using MinificationFilter = gl::MinificationFilter;

private:
	Id _id = 0;

public:
	Sampler() = default;

	Sampler(const Sampler&) = delete;
	Sampler& operator= (const Sampler&) = delete;

	inline Sampler(Sampler&& right) noexcept : _id(right._id) { right._id = 0; }
	
	inline Sampler& operator= (Sampler&& right) noexcept
	{
		std::destroy_at(this);
		return *std::construct_at<Sampler, Sampler&&>(this, std::move(right));
	}

	inline ~Sampler() { destroy(); }

public:
	constexpr bool isCreated() const { return _id != 0; }
	constexpr Id getId() const { return _id; }

	inline void activate(GLint textureUnit = 0) { if (checkIsCreated()) glBindSampler(GL_TEXTURE0 + textureUnit, _id); }

	inline void setFilter(MagnificationFilter filter) { if (checkIsCreated()) glSamplerParameteri(_id, GL_TEXTURE_MAG_FILTER, GLint(filter)); }
	inline void setFilter(MinificationFilter filter) { if (checkIsCreated()) glSamplerParameteri(_id, GL_TEXTURE_MIN_FILTER, GLint(filter)); }

	inline void setRepeat(bool repeat)
	{
		if (checkIsCreated())
		{
			GLint param = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
			glSamplerParameteri(_id, GL_TEXTURE_WRAP_S, param);
			glSamplerParameteri(_id, GL_TEXTURE_WRAP_T, param);
		}
	}

	bool create(bool repeat = true);

	void destroy();

private:
	inline bool checkIsCreated()
	{
		if (!isCreated())
		{
			logger::error("Attempting to access non loaded sampler!");
			return false;
		}
		return true;
	}
};
