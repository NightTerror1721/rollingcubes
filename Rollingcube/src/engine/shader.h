#pragma once

#include <string>
#include <optional>

#include "core/gl.h"

class Shader
{
public:
	static constexpr GLuint vertices_array_attrib_index = 0;
	static constexpr GLuint uvs_array_attrib_index = 1;
	static constexpr GLuint normals_array_attrib_index = 2;
	static constexpr GLuint tangents_array_attrib_index = 3;
	static constexpr GLuint bitangents_array_attrib_index = 4;
	static constexpr GLuint colors_array_attrib_index = 5;
	static constexpr GLuint indices_array_attrib_index = 6;

private:
	static constexpr GLuint invalid_id = 0;

	GLuint _programId = invalid_id;

public:
	Shader() = default;
	Shader(const Shader&) = delete;
	Shader(Shader&&) noexcept = default;

	Shader& operator= (const Shader&) = delete;
	Shader& operator= (Shader&&) noexcept = delete;


	~Shader();

	bool load(std::string_view vertexShaderFilename, std::string_view fragmentShaderFilename);


	inline GLuint programId() const { return _programId; }

	inline void activate() const { if (_programId != invalid_id) glUseProgram(_programId); }
	inline void deactivate() const { glUseProgram(invalid_id); }

private:
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
};
