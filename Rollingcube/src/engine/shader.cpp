#include "shader.h"

#include "utils/logger.h"
#include "utils/shader_constants.h"

#include "lua/module.h"
#include "utils/lualib_constants.h"


bool Shader::loadFromFile(std::string_view filename, Type type)
{
	destroy();

	std::ifstream sfile = std::ifstream(std::string(filename));
	if (!sfile)
	{
		logger::error("Cannot read shader file {}.", filename);
		return false;
	}

	std::stringstream ss;
	ss << sfile.rdbuf();
	std::string scode = ss.str();
	sfile.close();
	auto code = scode.c_str();


	_id = glCreateShader(static_cast<GLenum>(type));
	glShaderSource(_id, 1, &code, nullptr);
	glCompileShader(_id);

	GLint compilationStatus;
	glGetShaderiv(_id, GL_COMPILE_STATUS, &compilationStatus);
	if (compilationStatus == GL_FALSE)
	{
		logger::error("Error! Shader file {} wasn't compiled!", filename);

		GLint logLen;
		glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			GLchar* logMessage = new GLchar[logLen];
			glGetShaderInfoLog(_id, logLen, nullptr, logMessage);
			logger::error("The compiler returned: {}", logMessage);
			delete[] logMessage;
		}

		return false;
	}

	_type = type;
	_compiled = true;

	return true;
}

void Shader::destroy()
{
	if (isCreated())
		glDeleteShader(_id);

	_id = 0;
	_type = Type(0);
	_compiled = false;
}


DedicatedShaderManager<Shader::Type::Vertex> ShaderManager::VertexShaderManager = DedicatedShaderManager<Shader::Type::Vertex>();
DedicatedShaderManager<Shader::Type::Fragment> ShaderManager::FragmentShaderManager = DedicatedShaderManager<Shader::Type::Fragment>();
DedicatedShaderManager<Shader::Type::Geometry> ShaderManager::GeometryShaderManager = DedicatedShaderManager<Shader::Type::Geometry>();







bool ShaderProgram::link()
{
	if (!isCreated() || isLinked())
		return false;

	glLinkProgram(_id);

	GLint status;
	glGetProgramiv(_id, GL_LINK_STATUS, &status);
	_linked = status == GL_TRUE;

	if (!isLinked())
	{
		logger::error("Error! Shader program wasn't linked!");

		GLint logLen;
		glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			GLchar* logMessage = new GLchar[logLen];
			glGetProgramInfoLog(_id, logLen, nullptr, logMessage);
			logger::error("The linker returned: {}", logMessage);
			delete[] logMessage;
		}
	}

	return _linked;
}

void ShaderProgram::destroy()
{
	if (isCreated())
		glDeleteProgram(_id);

	_id = 0;
	_linked = false;
	_uniforms.clear();
}



ShaderProgramManager ShaderProgramManager::Instance = ShaderProgramManager();

ShaderProgramManager::ShaderProgramManager() :
	Manager(nullptr), _internalsCache()
{
	_internalsCache.resize(constants::shader::internals::count);
}

ShaderProgramManager::Reference ShaderProgramManager::load(
	const IdType& id,
	const std::string_view vertexShaderPath,
	std::string_view fragmentShaderPath,
	std::string_view geometryShaderPath
) {
	Reference program = get(id);
	if (program)
		return program;

	Shader::Ref vertex = ShaderManager::vertex().load(vertexShaderPath);
	Shader::Ref fragment = ShaderManager::fragment().load(fragmentShaderPath);
	Shader::Ref geometry = geometryShaderPath.empty() ? Shader::Ref() : ShaderManager::geometry().load(geometryShaderPath);

	if (!vertex || !fragment)
		return nullptr;

	program = emplace(id);
	if (!program)
		return nullptr;

	program->create();

	program->addShader(vertex);
	program->addShader(fragment);
	if (geometry)
		program->addShader(geometry);

	if (!program->link())
	{
		destroy(id);
		return nullptr;
	}

	return program;
}

void ShaderProgramManager::loadInternalShaders()
{
	using namespace constants::shader::internals;
	static constinit bool init = false;

	if (!init)
	{
		init = true;
		for (std::size_t i = 0; i < count; ++i)
			load(shaders[i]);
	}
}

#define GET_INTERNAL_SHADER(_Name)											\
	using namespace constants::shader;										\
																			\
	ShaderProgram::Ref ref = _internalsCache[(_Name).index];				\
	if (!ref)																\
		ref = (_internalsCache[(_Name).index] = get((_Name).name));			\
																			\
	return ref

ShaderProgramManager::Reference ShaderProgramManager::getLightningShaderProgram()
{
	GET_INTERNAL_SHADER(lightning);
}

ShaderProgramManager::Reference ShaderProgramManager::getFreetypeFontShaderProgram()
{
	GET_INTERNAL_SHADER(freetype_font);
}

ShaderProgramManager::Reference ShaderProgramManager::getSkyShaderProgram()
{
	GET_INTERNAL_SHADER(sky);
}

ShaderProgramManager::Reference ShaderProgramManager::getLinesShaderProgram()
{
	GET_INTERNAL_SHADER(lines);
}

#undef GET_INTERNAL_SHADER


ShaderProgramUniform::ShaderProgramUniform(std::string_view name, ShaderProgram& shaderProgram) :
	_name(name),
	_program(std::addressof(shaderProgram)),
	_location(glGetUniformLocation(shaderProgram.getId(), name.data()))
{
	if (_location == -1)
		logger::warn("Uniform with name {} does not exist, setting it will fail!", name);
}



void ShaderProgram::setUniformMaterial(const Material& material)
{
	using namespace constants::uniform::material;
	using namespace constants::uniform::flags;

	getUniform(ambientColor()) = material.getAmbientColor();
	getUniform(diffuseColor()) = material.getDiffuseColor();
	getUniform(specularColor()) = material.getSpecularColor();

	if (material.getDiffuseTexture() != nullptr)
	{
		material.getDiffuseTexture()->activate(0);
		getUniform(diffuseTexture()) = 0;
	}

	if (material.getSpecularTexture() != nullptr)
	{
		material.getSpecularTexture()->activate(1);
		getUniform(specularTexture()) = 1;
	}

	if (material.getNormalsTexture() != nullptr)
	{
		material.getNormalsTexture()->activate(2);
		getUniform(normalsTexture()) = 2;
		getUniform(useNormalMapping()) = true;
	}
	else
		getUniform(useNormalMapping()) = false;

	getUniform(shininess()) = material.getShininess();
	getUniform(opacity()) = material.getOpacity();
}

void ShaderProgram::setUniformStaticLightsCount(GLint lightCount)
{
	using namespace constants::uniform::static_lights;
	getUniform(count()) = glm::clamp(lightCount, 0, GLint(StaticLightContainer::maxStaticLights));
}

void ShaderProgram::setUniformStaticLight(const Light& light, GLint index)
{
	using namespace constants::uniform::static_lights;

	getUniform(position(index)) = light.getPosition();
	getUniform(ambientColor(index)) = light.getAmbientColor();
	getUniform(diffuseColor(index)) = light.getDiffuseColor();
	getUniform(specularColor(index)) = light.getSpecularColor();
	getUniform(constant(index)) = light.getConstantAttenuation();
	getUniform(linear(index)) = light.getLinearAttenuation();
	getUniform(quadratic(index)) = light.getQuadraticAttenuation();
	getUniform(intensity(index)) = light.getIntensity();
}

void ShaderProgram::setUniformMainStaticLight(const Light& light)
{
	using namespace constants::uniform::main_static_light;

	getUniform(useMainPointLight()) = true;
	getUniform(position()) = light.getPosition();
	getUniform(ambientColor()) = light.getAmbientColor();
	getUniform(diffuseColor()) = light.getDiffuseColor();
	getUniform(specularColor()) = light.getSpecularColor();
	getUniform(constant()) = light.getConstantAttenuation();
	getUniform(linear()) = light.getLinearAttenuation();
	getUniform(quadratic()) = light.getQuadraticAttenuation();
	getUniform(intensity()) = light.getIntensity();
}

void ShaderProgram::setUniformDisabledMainStaticLight()
{
	using namespace constants::uniform::main_static_light;

	getUniform(useMainPointLight()) = false;
}

void ShaderProgram::setUniformDirectionalLight(const DirectionalLight& light)
{
	using namespace constants::uniform::directional_light;

	getUniform(direction()) = light.getDirection();
	getUniform(ambientColor()) = light.getAmbientColor();
	getUniform(diffuseColor()) = light.getDiffuseColor();
	getUniform(specularColor()) = light.getSpecularColor();
	getUniform(intensity()) = light.getIntensity();
}









namespace lua::lib
{
	namespace LUA_shader { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerShaderLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::shader,
			&LUA_shader::registerToLua,
			{ ::lua::lib::names::geometry });
	}
}

namespace lua::lib::LUA_shader
{

	static Shader::Id getId(const ShaderProgram* self) { return self->getId(); }

	
	template <typename _Ty> requires requires(const ShaderProgram::Uniform& u, _Ty value) { { u = value }; }
	static void setUniformValue(const ShaderProgram* shader, const char* name, _Ty value)
	{
		shader->getUniform(name) = value;
	}

	template <typename _Ty> requires requires(const ShaderProgram::Uniform& u, const _Ty& value) { { u = value }; }
	static void setUniformRef(const ShaderProgram* shader, const char* name, const _Ty& value)
	{
		shader->getUniform(name) = value;
	}

	template <typename _Ty> requires requires(const ShaderProgram::Uniform& u, const std::vector<_Ty>& value) { { u = value }; }
	static void setUniformArray(const ShaderProgram* shader, const char* name, LuaRef value)
	{
		if (!value.isTable())
		{
			lua::utils::error("Expected valid table, but found {}.", value.tostring());
			return;
		}

		std::vector<_Ty> v;
		int len = value.length();
		if (len > 0)
		{
			v.reserve(len);
			for (int i = 0; i < len; ++i)
				v.push_back(value.rawget(i).cast<_Ty>().value());

			shader->getUniform(name) = v;
		}
	}

	static bool loadShaderProgram(
		const std::string& name,
		const char* vertexShaderPath,
		const char* fragmentShaderPath,
		LuaRef geometryShaderPathLUA)
	{
		ShaderProgramManager& man = ShaderProgramManager::instance();

		if (man.contains(name))
		{
			lua::utils::error("ShaderProgram '{}' already exists.", name);
			return false;
		}

		std::string geometryShaderPath = "";
		if (geometryShaderPathLUA.isString())
			geometryShaderPath = geometryShaderPathLUA.cast<std::string>().value();

		ShaderProgram::Ref ref = man.load(name, vertexShaderPath, fragmentShaderPath, geometryShaderPath);
		return ref != nullptr;
	}

	static ShaderProgram* get(const std::string& name)
	{
		ShaderProgram::Ref ref = ShaderProgramManager::instance().get(name);
		if (ref == nullptr)
		{
			lua::utils::error("ShaderProgram '{}' not found.", name);
			return nullptr;
		}
		return &ref;
	}

	static bool exists(const std::string& name)
	{
		return ShaderProgramManager::instance().contains(name);
	}

	static constinit struct DefaultShadersPool {} DefaultShaders = {};

	static DefaultShadersPool* getShaderProgramDefaults() { return &DefaultShaders; }
	static ShaderProgram* getLightningShaderProgram(const DefaultShadersPool*) { return &ShaderProgramManager::instance().getLightningShaderProgram(); }
	static ShaderProgram* getFreetypeFontShaderProgram(const DefaultShadersPool*) { return &ShaderProgramManager::instance().getFreetypeFontShaderProgram(); }
	static ShaderProgram* getSkyShaderProgram(const DefaultShadersPool*) { return &ShaderProgramManager::instance().getSkyShaderProgram(); }
	static ShaderProgram* getLinesShaderProgram(const DefaultShadersPool*) { return &ShaderProgramManager::instance().getLinesShaderProgram(); }




	static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		namespace meta = ::lua::metamethod;

		root = root.beginClass<DefaultShadersPool>("DefaultShadersPool")
			.addProperty("lightning", &getLightningShaderProgram)
			.addProperty("freetypeFont", &getFreetypeFontShaderProgram)
			.addProperty("sky", &getSkyShaderProgram)
			.addProperty("lines", &getLinesShaderProgram)
			.endClass();

		auto clss = root.beginClass<ShaderProgram>("ShaderProgram");
		clss
			// Properties //
			.addProperty("id", &getId)
			// Functions//
			.addFunction("isCreated", &ShaderProgram::isCreated)
			.addFunction("isValid", &ShaderProgram::isCreated)
			.addFunction("use", &ShaderProgram::use)
			.addFunction("setFloat", &setUniformValue<GLfloat>)
			.addFunction("setFloatArray", &setUniformArray<GLfloat>)
			.addFunction("setInt", &setUniformValue<GLint>)
			.addFunction("setIntArray", &setUniformArray<GLint>)
			.addFunction("setUnsignedInt", &setUniformValue<GLuint>)
			.addFunction("setUnsignedIntArray", &setUniformArray<GLuint>)
			.addFunction("setBoolean", &setUniformValue<bool>)
			.addFunction("setVec2", &setUniformRef<glm::vec2>)
			.addFunction("setVec2Array", &setUniformArray<glm::vec2>)
			.addFunction("setVec3", &setUniformRef<glm::vec3>)
			.addFunction("setVec3Array", &setUniformArray<glm::vec3>)
			.addFunction("setVec4", &setUniformRef<glm::vec4>)
			.addFunction("setVec4Array", &setUniformArray<glm::vec4>)
			.addFunction("setMat4", &setUniformRef<glm::mat4>)
			.addFunction("setMat4Array", &setUniformArray<glm::mat4>)
			// Static Functions //
			.addStaticFunction("load", &loadShaderProgram)
			.addStaticFunction("get", &get)
			.addStaticFunction("exists", &exists)
			// Static properties //
			.addStaticProperty("defaults", &getShaderProgramDefaults)
			;

		root = clss.endClass();
		return true;
	}
}
