#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

#include "core/gl.h"
#include "math/glm.h"
#include "utils/manager.h"
#include "utils/resources.h"

#include "light.h"
#include "material.h"


enum class ShaderType : GLenum
{
	Vertex = GL_VERTEX_SHADER,
	Fragment = GL_FRAGMENT_SHADER,
	Geometry = GL_GEOMETRY_SHADER
};

class Shader
{
public:
	using Id = GLuint;
	using Type = ShaderType;
	using Ref = ManagerReference<Shader>;

private:
	Id _id = 0;
	Type _type = Type(0);
	bool _compiled = false;

public:
	Shader() = default;

	Shader(const Shader&) = delete;
	Shader& operator= (const Shader&) = delete;

	inline ~Shader() { destroy(); }

public:
	constexpr bool isCreated() const { return _id != 0; }
	constexpr Id getId() const { return _id; }
	constexpr Type getType() const { return _type; }
	constexpr bool isCompiled() const { return _compiled; }

	bool loadFromFile(std::string_view filename, Type type);

	void destroy();
};


class ShaderManager;

template <Shader::Type _Type>
class DedicatedShaderManager : public Manager<Shader, std::string>
{
public:
	friend ShaderManager;

	inline Reference load(const std::string_view& filename)
	{
		Path path = std::filesystem::absolute(resources::shaders / filename);
		Reference ref = get(path.string());
		if (ref)
			return ref;

		ref = emplace(path.string());
		if (!ref)
			return nullptr;

		if (!ref->loadFromFile(path.string(), _Type))
		{
			destroy(path.string());
			return nullptr;
		}

		return ref;
	}

private:
	inline explicit DedicatedShaderManager() : Manager(nullptr) {}
};

class ShaderManager
{
private:
	ShaderManager() = delete;
	~ShaderManager() = delete;

	static DedicatedShaderManager<Shader::Type::Vertex> VertexShaderManager;
	static DedicatedShaderManager<Shader::Type::Fragment> FragmentShaderManager;
	static DedicatedShaderManager<Shader::Type::Geometry> GeometryShaderManager;

public:
	static inline DedicatedShaderManager<Shader::Type::Vertex>& vertex() { return VertexShaderManager; }
	static inline DedicatedShaderManager<Shader::Type::Fragment>& fragment() { return FragmentShaderManager; }
	static inline DedicatedShaderManager<Shader::Type::Geometry>& geometry() { return GeometryShaderManager; }
};




class ShaderProgramUniform;

class ShaderProgram
{
public:
	using Id = GLuint;
	using Uniform = ShaderProgramUniform;
	using Ref = ManagerReference<ShaderProgram>;

private:
	Id _id = 0;
	bool _linked = false;
	std::unordered_map<std::string, Uniform> _uniforms = {};

public:
	ShaderProgram() = default;

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator= (const ShaderProgram&) = delete;

	inline ~ShaderProgram() { destroy(); }

public:
	constexpr bool isCreated() const { return _id != 0; }
	constexpr Id getId() const { return _id; }
	constexpr bool isLinked() const { return _linked; }

	Uniform& getUniform(std::string_view name);
	inline const Uniform& getUniform(std::string_view name) const { return _uniforms.at(std::string(name)); }

	inline Uniform& operator[] (std::string_view name) { return getUniform(name); }

	inline bool addShader(Shader::Ref shader) const
	{
		if (!shader->isCompiled())
			return false;

		glAttachShader(_id, shader->getId());
		return true;
	}

	inline void use() { if (isLinked()) glUseProgram(_id); }

	inline void notUse() { glUseProgram(0); }

	void create() { _id = glCreateProgram(); }

	bool link();

	void destroy();

public:
	void setUniformMaterial(const Material& material);

	void setUniformStaticLightsCount(GLint count);
	void setUniformStaticLight(const Light& light, GLint index);

	void setUniformDirectionalLight(const DirectionalLight& light);
	
public:
	inline void setUniformStaticLights(const StaticLightContainer& lights)
	{
		const GLint len = GLint(lights.size());
		setUniformStaticLightsCount(len);

		for (GLint i = 0; i < len; ++i)
			setUniformStaticLight(lights[i], i);
	}
};


class ShaderProgramManager : public Manager<ShaderProgram>
{
private:
	static ShaderProgramManager Instance;

	std::vector<Reference> _internalsCache;

public:
	Reference load(const IdType& id, const std::string_view vertexShaderPath, std::string_view fragmentShaderPath, std::string_view geometryShaderPath = "");

	void loadInternalShaders();

	static inline ShaderProgramManager& instance() { return Instance; }

public:
	inline bool contains(std::string_view id) const { return contains(IdType(id)); }

	inline Reference get(std::string_view id) { return Manager<ShaderProgram>::get(IdType(id)); }
	inline ConstReference get(std::string_view id) const { return Manager<ShaderProgram>::get(IdType(id)); }

	inline void clear() override { _internalsCache.clear(), Manager<ShaderProgram>::clear(); }

public:
	Reference getLightningShaderProgram();

private:
	explicit ShaderProgramManager();
};



class ShaderProgramUniform
{
public:
	using Location = GLint;

private:
	std::string _name = {};
	ShaderProgram* _program = nullptr;
	Location _location = -1;

public:
	ShaderProgramUniform() = default;
	ShaderProgramUniform(const ShaderProgramUniform&) = default;
	ShaderProgramUniform(ShaderProgramUniform&&) noexcept = default;
	~ShaderProgramUniform() = default;

	ShaderProgramUniform& operator= (const ShaderProgramUniform&) = default;
	ShaderProgramUniform& operator= (ShaderProgramUniform&&) noexcept = default;

	ShaderProgramUniform(std::string_view name, ShaderProgram& shaderProgram);

public:
	inline void set(GLfloat value) const { glUniform1f(_location, value); }
	inline void set(const GLfloat* values, GLsizei count) const { glUniform1fv(_location, count, values); }
	inline void set(const std::vector<GLfloat>& v) const { glUniform1fv(_location, GLsizei(v.size()), v.data()); }
	inline const ShaderProgramUniform& operator= (GLfloat value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<GLfloat>& value) const { return set(value), *this; }

	inline void set(GLint value) const { glUniform1i(_location, value); }
	inline void set(const GLint* values, GLsizei count) const { glUniform1iv(_location, count, values); }
	inline void set(const std::vector<GLint>& v) const { glUniform1iv(_location, GLsizei(v.size()), v.data()); }
	inline const ShaderProgramUniform& operator= (GLint value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<GLint>& value) const { return set(value), *this; }

	inline void set(GLuint value) const { glUniform1ui(_location, value); }
	inline void set(const GLuint* values, GLsizei count) const { glUniform1uiv(_location, count, values); }
	inline void set(const std::vector<GLuint>& v) const { glUniform1uiv(_location, GLsizei(v.size()), v.data()); }
	inline const ShaderProgramUniform& operator= (GLuint value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<GLuint>& value) const { return set(value), *this; }


	inline void set(const glm::vec2& value) const { glUniform2f(_location, value.x, value.y); }
	inline void set(const glm::vec2* values, GLsizei count) const { glUniform2fv(_location, count, reinterpret_cast<const GLfloat*>(values)); }
	inline void set(const std::vector<glm::vec2>& v) const { glUniform2fv(_location, GLsizei(v.size()), reinterpret_cast<const GLfloat*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::vec2& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::vec2>& value) const { return set(value), *this; }

	inline void set(const glm::ivec2& value) const { glUniform2i(_location, value.x, value.y); }
	inline void set(const glm::ivec2* values, GLsizei count) const { glUniform2iv(_location, count, reinterpret_cast<const GLint*>(values)); }
	inline void set(const std::vector<glm::ivec2>& v) const { glUniform2iv(_location, GLsizei(v.size()), reinterpret_cast<const GLint*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::ivec2& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::ivec2>& value) const { return set(value), *this; }

	inline void set(const glm::uvec2& value) const { glUniform2ui(_location, value.x, value.y); }
	inline void set(const glm::uvec2* values, GLsizei count) const { glUniform2uiv(_location, count, reinterpret_cast<const GLuint*>(values)); }
	inline void set(const std::vector<glm::uvec2>& v) const { glUniform2uiv(_location, GLsizei(v.size()), reinterpret_cast<const GLuint*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::uvec2& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::uvec2>& value) const { return set(value), *this; }


	inline void set(const glm::vec3& value) const { glUniform3f(_location, value.x, value.y, value.z); }
	inline void set(const glm::vec3* values, GLsizei count) const { glUniform3fv(_location, count, reinterpret_cast<const GLfloat*>(values)); }
	inline void set(const std::vector<glm::vec3>& v) const { glUniform3fv(_location, GLsizei(v.size()), reinterpret_cast<const GLfloat*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::vec3& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::vec3>& value) const { return set(value), *this; }

	inline void set(const glm::ivec3& value) const { glUniform3i(_location, value.x, value.y, value.z); }
	inline void set(const glm::ivec3* values, GLsizei count) const { glUniform3iv(_location, count, reinterpret_cast<const GLint*>(values)); }
	inline void set(const std::vector<glm::ivec3>& v) const { glUniform3iv(_location, GLsizei(v.size()), reinterpret_cast<const GLint*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::ivec3& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::ivec3>& value) const { return set(value), *this; }

	inline void set(const glm::uvec3& value) const { glUniform3ui(_location, value.x, value.y, value.z); }
	inline void set(const glm::uvec3* values, GLsizei count) const { glUniform3uiv(_location, count, reinterpret_cast<const GLuint*>(values)); }
	inline void set(const std::vector<glm::uvec3>& v) const { glUniform3uiv(_location, GLsizei(v.size()), reinterpret_cast<const GLuint*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::uvec3& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::uvec3>& value) const { return set(value), *this; }


	inline void set(const glm::vec4& value) const { glUniform4f(_location, value.x, value.y, value.z, value.w); }
	inline void set(const glm::vec4* values, GLsizei count) const { glUniform4fv(_location, count, reinterpret_cast<const GLfloat*>(values)); }
	inline void set(const std::vector<glm::vec4>& v) const { glUniform4fv(_location, GLsizei(v.size()), reinterpret_cast<const GLfloat*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::vec4& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::vec4>& value) const { return set(value), *this; }

	inline void set(const glm::ivec4& value) const { glUniform4i(_location, value.x, value.y, value.z, value.w); }
	inline void set(const glm::ivec4* values, GLsizei count) const { glUniform4iv(_location, count, reinterpret_cast<const GLint*>(values)); }
	inline void set(const std::vector<glm::ivec4>& v) const { glUniform4iv(_location, GLsizei(v.size()), reinterpret_cast<const GLint*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::ivec4& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::ivec4>& value) const { return set(value), *this; }

	inline void set(const glm::uvec4& value) const { glUniform4ui(_location, value.x, value.y, value.z, value.w); }
	inline void set(const glm::uvec4* values, GLsizei count) const { glUniform4uiv(_location, count, reinterpret_cast<const GLuint*>(values)); }
	inline void set(const std::vector<glm::uvec4>& v) const { glUniform4uiv(_location, GLsizei(v.size()), reinterpret_cast<const GLuint*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::uvec4& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::uvec4>& value) const { return set(value), *this; }


	inline void set(const glm::mat3& value) const { glUniformMatrix3fv(_location, 1, false, reinterpret_cast<const GLfloat*>(std::addressof(value))); }
	inline void set(const glm::mat3* values, GLsizei count) const { glUniformMatrix3fv(_location, count, false, reinterpret_cast<const GLfloat*>(std::addressof(values))); }
	inline void set(const std::vector<glm::mat3>& v) const { glUniformMatrix3fv(_location, GLsizei(v.size()), false, reinterpret_cast<const GLfloat*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::mat3& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::mat3>& value) const { return set(value), *this; }


	inline void set(const glm::mat4& value) const { glUniformMatrix4fv(_location, 1, false, reinterpret_cast<const GLfloat*>(std::addressof(value))); }
	inline void set(const glm::mat4* values, GLsizei count) const { glUniformMatrix4fv(_location, count, false, reinterpret_cast<const GLfloat*>(std::addressof(values))); }
	inline void set(const std::vector<glm::mat4>& v) const { glUniformMatrix4fv(_location, GLsizei(v.size()), false, reinterpret_cast<const GLfloat*>(v.data())); }
	inline const ShaderProgramUniform& operator= (const glm::mat4& value) const { return set(value), *this; }
	inline const ShaderProgramUniform& operator= (const std::vector<glm::mat4>& value) const { return set(value), *this; }
};


inline ShaderProgram::Uniform& ShaderProgram::getUniform(std::string_view name)
{
	auto it = _uniforms.find(std::string(name));
	if (it != _uniforms.end())
		return it->second;

	return (_uniforms[std::string(name)] = Uniform(name, *this));
}
