#include "shader.h"

#include <unordered_map>
#include <string>

#include "utils/io_utils.h"
#include "utils/logger.h"
#include "utils/resources.h"


std::optional<std::string> Shader::readShader(const std::string_view& filename)
{
	std::ifstream sfile = std::ifstream(std::string(filename));
	if (!sfile)
	{
		logger::error("Cannot read shader file {}.", filename);
		return {};
	}

	std::stringstream ss;
	ss << sfile.rdbuf();
	std::string code = ss.str();
	sfile.close();

	return code;
}

void Shader::compileShader(GLuint shaderId, const std::string& code)
{
	auto code_ptr = code.c_str();
	glShaderSource(shaderId, 1, &code_ptr, nullptr);
	glCompileShader(shaderId);
}

bool Shader::checkShader(GLuint shaderId)
{
	GLint result = GL_FALSE;
	GLint infoLogLen;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLen);

	if (infoLogLen > 0)
	{
		GLchar* errorMsg = new GLchar[infoLogLen + 1];
		glGetShaderInfoLog(shaderId, infoLogLen, nullptr, errorMsg);
		logger::error("Shader compilation error: {}.", errorMsg);
		delete[] errorMsg;
		return false;
	}

	return true;
}

std::optional<GLuint> Shader::loadShader(bool isVertexShader, const std::string_view& filename)
{
	GLuint shaderId = isVertexShader ? createVertexShader() : createFragmentShader();

	logger::info("Reading '{}' {} shader source code.", filename, (isVertexShader ? "vertex" : "fragment"));
	auto code = readShader(filename);
	if (!code.has_value())
	{
		deleteShader(shaderId);
		return {};
	}

	logger::info("Compiling '{}' {} shader source code.", filename, (isVertexShader ? "vertex" : "fragment"));
	compileShader(shaderId, code.value());
	if (!checkShader(shaderId))
	{
		deleteShader(shaderId);
		return {};
	}

	return shaderId;
}

void Shader::linkProgram(GLuint programId, GLuint vertexShaderId, GLuint fragmentShaderId)
{
	attachShader(programId, vertexShaderId);
	attachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
}

bool Shader::checkProgram(GLuint programId)
{
	GLint result = GL_FALSE;
	GLint infoLogLen;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLen);

	if (infoLogLen > 0)
	{
		GLchar* errorMsg = new GLchar[infoLogLen + 1];
		glGetProgramInfoLog(programId, infoLogLen, nullptr, errorMsg);
		logger::error("Shader compilation error: {}.", errorMsg);
		delete[] errorMsg;
		return false;
	}

	return true;
}




Shader::~Shader()
{
	if(_programId != invalid_id)
		deleteProgram(_programId);
}

void Shader::release()
{
	if (_programId != invalid_id)
		deleteProgram(_programId);

	_programId = invalid_id;
	clearCache();
}

void Shader::clearCache() const
{
	_uniformCache.clear();
}

bool Shader::load(std::string_view vertexShaderFilename, std::string_view fragmentShaderFilename)
{
	auto vertexShaderId = loadShader(true, vertexShaderFilename);
	if (!vertexShaderId.has_value())
		return false;

	auto fragmentShaderId = loadShader(false, fragmentShaderFilename);
	if (!fragmentShaderId.has_value())
	{
		deleteShader(vertexShaderId.value());
		return false;
	}

	logger::info("Creating Shader Program");
	GLuint programId = createProgram();
	linkProgram(programId, vertexShaderId.value(), fragmentShaderId.value());
	if (!checkProgram(programId))
	{
		deleteShader(vertexShaderId.value());
		deleteShader(fragmentShaderId.value());
		deleteProgram(programId);
		return false;
	}

	detachShader(programId, vertexShaderId.value());
	detachShader(programId, fragmentShaderId.value());

	deleteShader(vertexShaderId.value());
	deleteShader(fragmentShaderId.value());

	release();

	_programId = programId;
	return true;
}

Shader::uniform_index_t Shader::getUniformLocation(std::string_view vs_name) const
{
	auto name = std::string(vs_name);
	auto it = _uniformCache.find(name);
	if (it != _uniformCache.end())
		return it->second;

	auto loc = glGetUniformLocation(_programId, name.c_str());
	if (loc < 0)
	{
		logger::error("Cannot get uniform '{}' location.", name);
		return -1;
	}

	_uniformCache.emplace(name, loc);
	return loc;
}





namespace shaders::manager
{
	struct ShaderEntry
	{
		std::string vertex;
		std::string fragment;
	};

	static std::unordered_map<std::string, ShaderEntry> ShadersNames;
	static bool ShadersNamesLoaded = false;

	static std::unordered_map<std::string, std::shared_ptr<Shader>> ShadersCache;


	static void loadNames()
	{
		if (ShadersNamesLoaded)
			return;

		try
		{
			JsonValue json = resources::shaders.readJson("config.json");
			if (json.is_object())
			{
				const JsonObject& shadersMap = json.get_ref<const JsonObject&>();
				for (const auto& e : shadersMap)
				{
					std::string_view shaderName;
					try
					{
						shaderName = e.first;
						if (e.second.is_object())
						{
							const JsonObject& rawShaderEntry = e.second.get_ref<const JsonObject&>();
							auto vertexIt = rawShaderEntry.find("vertex");
							if (vertexIt == rawShaderEntry.end())
								throw std::exception("Vertex Shader not found.");
							auto fragmentIt = rawShaderEntry.find("fragment");
							if (fragmentIt == rawShaderEntry.end())
								throw std::exception("Fragment Shader not found.");

							ShaderEntry entry = {
								.vertex = vertexIt->second.get_ref<const std::string&>(),
								.fragment = fragmentIt->second.get_ref<const std::string&>()
							};
							ShadersNames.insert({ shaderName.data(), entry });
						}
					}
					catch (const std::exception& ex)
					{
						logger::error("Exception during shader name {} load: {}", shaderName, ex.what());
					}
				}
			}
		}
		catch (const std::exception& ex)
		{
			logger::fatal("Exception during shader names load: {}", ex.what());
		}

		ShadersNamesLoaded = true;
	}
}


const std::shared_ptr<Shader>& Shader::get(std::string_view name)
{
	if (!shaders::manager::ShadersNamesLoaded)
		shaders::manager::loadNames();

	auto it = shaders::manager::ShadersCache.find(name.data());
	if (it != shaders::manager::ShadersCache.end())
		return it->second;

	auto namesIt = shaders::manager::ShadersNames.find(name.data());
	if (namesIt == shaders::manager::ShadersNames.end())
	{
		logger::error("Shader {} not found!", name);
		return nullptr;
	}

	std::shared_ptr<Shader> ptr = std::make_shared<Shader>();
	if (!ptr->load(namesIt->second.vertex, namesIt->second.fragment))
		return nullptr;

	auto result = shaders::manager::ShadersCache.insert({ name.data(), std::move(ptr) });
	if (!result.second)
	{
		logger::error("Cannot load {} shader. Unknown error.", name);
		return nullptr;
	}

	return result.first->second;
}

void Shader::loadAll()
{
	if (!shaders::manager::ShadersNamesLoaded)
		shaders::manager::loadNames();

	for (auto e : shaders::manager::ShadersNames)
		get(e.first);
}
