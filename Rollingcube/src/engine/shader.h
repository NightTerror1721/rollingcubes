#pragma once

#include <string>
#include <unordered_map>
#include <optional>

#include "core/gl.h"
#include "math/glm.h"
#include "math/color.h"

#include "material.h"

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
	static constexpr std::string_view materialDiffuseColor = "materialDiffuseColor";
	static constexpr std::string_view materialAmbientColor = "materialAmbientColor";
	static constexpr std::string_view materialSpecularColor = "materialSpecularColor";

	static constexpr std::string_view materialDiffuseCoeficient = "materialDiffuseK";
	static constexpr std::string_view materialAmbientCoeficient = "materialAmbientK";
	static constexpr std::string_view materialSpecularCoeficient = "materialSpecularK";

	static constexpr std::string_view materialShininess = "materialShininess";


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
	template <typename _Ty> void setUniform(std::string_view name, const _Ty& val0) const;

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



	void setUniformMaterial(const Material& material) const;
};



template <> inline void Shader::setUniform<GLfloat>(std::string_view name, const GLfloat& value) const
{
	glUniform1f(getUniformLocation(name), value);
}


template <> inline void Shader::setUniform<GLint>(std::string_view name, const GLint& value) const
{
	glUniform1i(getUniformLocation(name), value);
}


template <> inline void Shader::setUniform<GLuint>(std::string_view name, const GLuint& value) const
{
	glUniform1ui(getUniformLocation(name), value);
}



template <> inline void Shader::setUniform<glm::vec2>(std::string_view name, const glm::vec2& value) const
{
	glUniform2f(getUniformLocation(name), value.x, value.y);
}

template <> inline void Shader::setUniform<glm::vec3>(std::string_view name, const glm::vec3& value) const
{
	glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

template <> inline void Shader::setUniform<glm::vec4>(std::string_view name, const glm::vec4& value) const
{
	glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

template <> inline void Shader::setUniform<const Color&>(std::string_view name, const Color& color) const
{
	setUniform<glm::vec4>(name, glm::vec4(color));
}


template <> inline void Shader::setUniform<const glm::ivec2&>(std::string_view name, const glm::ivec2& value) const
{
	glUniform2i(getUniformLocation(name), value.x, value.y);
}

template <> inline void Shader::setUniform<const glm::ivec3&>(std::string_view name, const glm::ivec3& value) const
{
	glUniform3i(getUniformLocation(name), value.x, value.y, value.z);
}

template <> inline void Shader::setUniform<const glm::ivec4&>(std::string_view name, const glm::ivec4& value) const
{
	glUniform4i(getUniformLocation(name), value.x, value.y, value.z, value.w);
}


template <> inline void Shader::setUniform<const glm::uvec2&>(std::string_view name, const glm::uvec2& value) const
{
	glUniform2ui(getUniformLocation(name), value.x, value.y);
}

template <> inline void Shader::setUniform<const glm::uvec3&>(std::string_view name, const glm::uvec3& value) const
{
	glUniform3ui(getUniformLocation(name), value.x, value.y, value.z);
}

template <> inline void Shader::setUniform<const glm::uvec4&>(std::string_view name, const glm::uvec4& value) const
{
	glUniform4ui(getUniformLocation(name), value.x, value.y, value.z, value.w);
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





inline void Shader::setUniformMaterial(const Material& material) const
{
	setUniform(materialDiffuseColor, material.getDiffuseColor());
	setUniform(materialAmbientColor, material.getAmbientColor());
	setUniform(materialSpecularColor, material.getSpecularColor());

	setUniform(materialDiffuseCoeficient, material.getDiffuseCoeficient());
	setUniform(materialAmbientCoeficient, material.getAmbientCoeficient());
	setUniform(materialSpecularCoeficient, material.getSpecularCoeficient());

	setUniform(materialShininess.data(), material.getShininess());
}
