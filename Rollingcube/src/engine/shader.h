#pragma once

#include <string>
#include <unordered_map>
#include <optional>

#include "core/gl.h"
#include "math/glm.h"
#include "math/color.h"

class Shader
{
public:
	static constexpr GLuint vertices_array_attrib_index = 0;
	static constexpr GLuint uvs_array_attrib_index = 1;
	static constexpr GLuint normals_array_attrib_index = 2;
	static constexpr GLuint tangents_array_attrib_index = 3;
	static constexpr GLuint bitangents_array_attrib_index = 4;
	static constexpr GLuint colors_array_attrib_index = 5;

private:
	using uniform_index_t = decltype(glGetUniformLocation(0, nullptr));

	static constexpr GLuint invalid_id = 0;

	GLuint _programId = invalid_id;

	mutable std::unordered_map<std::string, uniform_index_t> _uniformCache;


public:
	Shader() = default;
	Shader(const Shader&) = delete;
	Shader(Shader&&) noexcept = default;

	Shader& operator= (const Shader&) = delete;
	Shader& operator= (Shader&&) noexcept = delete;


	~Shader();

	void release();
	void clearCache() const;

	bool load(std::string_view vertexShaderFilename, std::string_view fragmentShaderFilename);


	inline GLuint programId() const { return _programId; }

	inline void bind() const { if (_programId != invalid_id) glUseProgram(_programId); }
	inline void unbind() const { glUseProgram(invalid_id); }

private:
	uniform_index_t getUniformLocation(std::string_view name) const;


	static std::optional<std::string> readShader(const std::string_view& filename);
	static void compileShader(GLuint shaderId, const std::string& code);
	static bool checkShader(GLuint shaderId);

	static std::optional<GLuint> loadShader(bool isVertexShader, const std::string_view& filename);
	
	static void linkProgram(GLuint programId, GLuint vertexShaderId, GLuint fragmentShaderId);
	static bool checkProgram(GLuint programId);


	static inline GLuint createVertexShader() { return glCreateShader(GL_VERTEX_SHADER); }
	static inline GLuint createFragmentShader() { return glCreateShader(GL_FRAGMENT_SHADER); }

	static inline GLuint createProgram() { return glCreateProgram(); }

	static inline void attachShader(GLuint programId, GLuint shaderId) { glAttachShader(programId, shaderId); }

	static inline void detachShader(GLuint programId, GLuint shaderId) { glDetachShader(programId, shaderId); }
	static inline void deleteShader(GLuint& shaderId) { glDeleteShader(shaderId), shaderId = invalid_id; }

	static inline void deleteProgram(GLuint& programId) { glDeleteProgram(programId), programId = invalid_id; }

public:
	template <typename _Ty> void setUniform(std::string_view name, _Ty val0) const;
	template <typename _Ty> void setUniform(std::string_view name, _Ty val0, _Ty val1) const;
	template <typename _Ty> void setUniform(std::string_view name, _Ty val0, _Ty val1, _Ty val2) const;
	template <typename _Ty> void setUniform(std::string_view name, _Ty val0, _Ty val1, _Ty val2, _Ty val3) const;

	template <typename _Ty> void setUniformArray(std::string_view name, const _Ty* array, GLsizei count) const;

	template <typename _Ty> void setUniformMatrixArray(std::string_view name, const _Ty* matrices, GLsizei count, GLboolean transpose = false) const;



	template <typename _Ty> requires
		requires(const Shader& shader, std::string_view name, const _Ty* array, GLsizei count)
		{
			{ shader.setUniformArray(name, array, count) };
		}
	inline void setUniformArray(std::string_view name, const std::vector<_Ty>& v) { setUniformArray(name, v.data(), v.size()); }

	template <typename _Ty> requires
		requires(const Shader& shader, std::string_view name, const _Ty* matrices, GLsizei count, GLboolean transpose)
	{
		{ shader.setUniformMatrixArray(name, matrices, count, transpose) };
	}
	inline void setUniformMatrixArray(std::string_view name, const std::vector<_Ty>& v, bool transpose = false)
	{
		setUniformMatrixArray(name, v.data(), v.size(), transpose);
	}

	template <typename _Ty> requires
		requires(const Shader& shader, std::string_view name, const _Ty* matrices, GLsizei count, GLboolean transpose)
	{
		{ shader.setUniformMatrixArray(name, matrices, count, transpose) };
	}
	inline void setUniformMatrix(std::string_view name, const _Ty& matrix, bool transpose = false)
	{
		setUniformMatrixArray(name, std::addressof(matrix), 1, transpose);
	}
};



template <> inline void Shader::setUniform<GLfloat>(std::string_view name, GLfloat val0) const
{
	glUniform1f(getUniformLocation(name), val0);
}

template <> inline void Shader::setUniform<GLfloat>(std::string_view name, GLfloat val0, GLfloat val1) const
{
	glUniform2f(getUniformLocation(name), val0, val1);
}

template <> inline void Shader::setUniform<GLfloat>(std::string_view name, GLfloat val0, GLfloat val1, GLfloat val2) const
{
	glUniform3f(getUniformLocation(name), val0, val1, val2);
}

template <> inline void Shader::setUniform<GLfloat>(std::string_view name, GLfloat val0, GLfloat val1, GLfloat val2, GLfloat val3) const
{
	glUniform4f(getUniformLocation(name), val0, val1, val2, val3);
}


template <> inline void Shader::setUniform<GLint>(std::string_view name, GLint val0) const
{
	glUniform1i(getUniformLocation(name), val0);
}

template <> inline void Shader::setUniform<GLint>(std::string_view name, GLint val0, GLint val1) const
{
	glUniform2i(getUniformLocation(name), val0, val1);
}

template <> inline void Shader::setUniform<GLint>(std::string_view name, GLint val0, GLint val1, GLint val2) const
{
	glUniform3i(getUniformLocation(name), val0, val1, val2);
}

template <> inline void Shader::setUniform<GLint>(std::string_view name, GLint val0, GLint val1, GLint val2, GLint val3) const
{
	glUniform4i(getUniformLocation(name), val0, val1, val2, val3);
}


template <> inline void Shader::setUniform<GLuint>(std::string_view name, GLuint val0) const
{
	glUniform1ui(getUniformLocation(name), val0);
}

template <> inline void Shader::setUniform<GLuint>(std::string_view name, GLuint val0, GLuint val1) const
{
	glUniform2ui(getUniformLocation(name), val0, val1);
}

template <> inline void Shader::setUniform<GLuint>(std::string_view name, GLuint val0, GLuint val1, GLuint val2) const
{
	glUniform3ui(getUniformLocation(name), val0, val1, val2);
}

template <> inline void Shader::setUniform<GLuint>(std::string_view name, GLuint val0, GLuint val1, GLuint val2, GLuint val3) const
{
	glUniform4ui(getUniformLocation(name), val0, val1, val2, val3);
}


template <> inline void Shader::setUniform<const glm::vec2&>(std::string_view name, const glm::vec2& v) const
{
	setUniform<GLfloat>(name, v.x, v.y);
}

template <> inline void Shader::setUniform<const glm::vec3&>(std::string_view name, const glm::vec3& v) const
{
	setUniform<GLfloat>(name, v.x, v.y, v.z);
}

template <> inline void Shader::setUniform<const glm::vec4&>(std::string_view name, const glm::vec4& v) const
{
	setUniform<GLfloat>(name, v.x, v.y, v.z, v.w);
}

template <> inline void Shader::setUniform<const Color&>(std::string_view name, const Color& color) const
{
	setUniform<glm::vec4>(name, glm::vec4(color));
}


template <> inline void Shader::setUniform<const glm::ivec2&>(std::string_view name, const glm::ivec2& v) const
{
	setUniform<GLint>(name, v.x, v.y);
}

template <> inline void Shader::setUniform<const glm::ivec3&>(std::string_view name, const glm::ivec3& v) const
{
	setUniform<GLint>(name, v.x, v.y, v.z);
}

template <> inline void Shader::setUniform<const glm::ivec4&>(std::string_view name, const glm::ivec4& v) const
{
	setUniform<GLint>(name, v.x, v.y, v.z, v.w);
}


template <> inline void Shader::setUniform<const glm::uvec2&>(std::string_view name, const glm::uvec2& v) const
{
	setUniform<GLuint>(name, v.x, v.y);
}

template <> inline void Shader::setUniform<const glm::uvec3&>(std::string_view name, const glm::uvec3& v) const
{
	setUniform<GLuint>(name, v.x, v.y, v.z);
}

template <> inline void Shader::setUniform<const glm::uvec4&>(std::string_view name, const glm::uvec4& v) const
{
	setUniform<GLuint>(name, v.x, v.y, v.z, v.w);
}


template <> inline void Shader::setUniformArray<GLfloat>(std::string_view name, const GLfloat* array, GLsizei count) const
{
	glUniform1fv(getUniformLocation(name), count, array);
}

template <> inline void Shader::setUniformArray<glm::vec2>(std::string_view name, const glm::vec2* array, GLsizei count) const
{
	glUniform2fv(getUniformLocation(name), count, &array[0][0]);
}

template <> inline void Shader::setUniformArray<glm::vec3>(std::string_view name, const glm::vec3* array, GLsizei count) const
{
	glUniform3fv(getUniformLocation(name), count, &array[0][0]);
}

template <> inline void Shader::setUniformArray<glm::vec4>(std::string_view name, const glm::vec4* array, GLsizei count) const
{
	glUniform4fv(getUniformLocation(name), count, &array[0][0]);
}


template <> inline void Shader::setUniformArray<GLint>(std::string_view name, const GLint* array, GLsizei count) const
{
	glUniform1iv(getUniformLocation(name), count, array);
}

template <> inline void Shader::setUniformArray<glm::ivec2>(std::string_view name, const glm::ivec2* array, GLsizei count) const
{
	glUniform2iv(getUniformLocation(name), count, &array[0][0]);
}

template <> inline void Shader::setUniformArray<glm::ivec3>(std::string_view name, const glm::ivec3* array, GLsizei count) const
{
	glUniform3iv(getUniformLocation(name), count, &array[0][0]);
}

template <> inline void Shader::setUniformArray<glm::ivec4>(std::string_view name, const glm::ivec4* array, GLsizei count) const
{
	glUniform4iv(getUniformLocation(name), count, &array[0][0]);
}


template <> inline void Shader::setUniformArray<GLuint>(std::string_view name, const GLuint* array, GLsizei count) const
{
	glUniform1uiv(getUniformLocation(name), count, array);
}

template <> inline void Shader::setUniformArray<glm::uvec2>(std::string_view name, const glm::uvec2* array, GLsizei count) const
{
	glUniform2uiv(getUniformLocation(name), count, &array[0][0]);
}

template <> inline void Shader::setUniformArray<glm::uvec3>(std::string_view name, const glm::uvec3* array, GLsizei count) const
{
	glUniform3uiv(getUniformLocation(name), count, &array[0][0]);
}

template <> inline void Shader::setUniformArray<glm::uvec4>(std::string_view name, const glm::uvec4* array, GLsizei count) const
{
	glUniform4uiv(getUniformLocation(name), count, &array[0][0]);
}


template <> inline void Shader::setUniformMatrixArray<glm::mat2>(std::string_view name, const glm::mat2* matrices, GLsizei count, GLboolean transpose) const
{
	glUniformMatrix2fv(getUniformLocation(name), count, transpose, &matrices[0][0][0]);
}

template <> inline void Shader::setUniformMatrixArray<glm::mat3>(std::string_view name, const glm::mat3* matrices, GLsizei count, GLboolean transpose) const
{
	glUniformMatrix3fv(getUniformLocation(name), count, transpose, &matrices[0][0][0]);
}

template <> inline void Shader::setUniformMatrixArray<glm::mat4>(std::string_view name, const glm::mat4* matrices, GLsizei count, GLboolean transpose) const
{
	glUniformMatrix4fv(getUniformLocation(name), count, transpose, &matrices[0][0][0]);
}

template <> inline void Shader::setUniformMatrixArray<glm::mat2x3>(std::string_view name, const glm::mat2x3* matrices, GLsizei count, GLboolean transpose) const
{
	glUniformMatrix2x3fv(getUniformLocation(name), count, transpose, &matrices[0][0][0]);
}

template <> inline void Shader::setUniformMatrixArray<glm::mat3x2>(std::string_view name, const glm::mat3x2* matrices, GLsizei count, GLboolean transpose) const
{
	glUniformMatrix3x2fv(getUniformLocation(name), count, transpose, &matrices[0][0][0]);
}

template <> inline void Shader::setUniformMatrixArray<glm::mat2x4>(std::string_view name, const glm::mat2x4* matrices, GLsizei count, GLboolean transpose) const
{
	glUniformMatrix2x4fv(getUniformLocation(name), count, transpose, &matrices[0][0][0]);
}

template <> inline void Shader::setUniformMatrixArray<glm::mat4x2>(std::string_view name, const glm::mat4x2* matrices, GLsizei count, GLboolean transpose) const
{
	glUniformMatrix4x2fv(getUniformLocation(name), count, transpose, &matrices[0][0][0]);
}

template <> inline void Shader::setUniformMatrixArray<glm::mat3x4>(std::string_view name, const glm::mat3x4* matrices, GLsizei count, GLboolean transpose) const
{
	glUniformMatrix3x4fv(getUniformLocation(name), count, transpose, &matrices[0][0][0]);
}

template <> inline void Shader::setUniformMatrixArray<glm::mat4x3>(std::string_view name, const glm::mat4x3* matrices, GLsizei count, GLboolean transpose) const
{
	glUniformMatrix4x3fv(getUniformLocation(name), count, transpose, &matrices[0][0][0]);
}
