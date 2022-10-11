#include "shader.h"

#include "utils/logger.h"
#include "utils/shader_constants.h"


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

	for (std::size_t i = 0; i < count; ++i)
		load(shaders[i]);
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

	getUniform(shininess()) = material.getShininess();
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

void ShaderProgram::setUniformDirectionalLight(const DirectionalLight& light)
{
	using namespace constants::uniform::directional_light;

	getUniform(direction()) = light.getDirection();
	getUniform(ambientColor()) = light.getAmbientColor();
	getUniform(diffuseColor()) = light.getDiffuseColor();
	getUniform(specularColor()) = light.getSpecularColor();
	getUniform(intensity()) = light.getIntensity();
}
