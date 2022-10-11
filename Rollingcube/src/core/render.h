#pragma once

#include "gl.h"
#include "vertex_array.h"


namespace gl
{
	inline void render(const VertexArrayObject& vao, GLenum mode = GL_TRIANGLES, GLint first = 0)
	{
		if (vao.isCreated())
		{
			vao.bind();

			const auto& attr = vao.getAttribute(vao.getVerticesAttributeId());
			if (attr.getElementCount() > 0)
				glDrawArrays(mode, first, attr.getElementCount());
		}
	}

	inline void render(const VertexArrayObject& vao, const EBO& ebo, GLenum mode = GL_TRIANGLES)
	{
		if (vao.isCreated() && ebo.isCreated() && !ebo.empty())
		{
			vao.bind();
			ebo.bind();
			glDrawElements(mode, ebo.getElementCount(), GL_UNSIGNED_INT, nullptr);
		}
	}
}
