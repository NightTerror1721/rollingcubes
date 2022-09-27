#include "shader.h"

#include "utils/io_utils.h"
#include "utils/logger.h"


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

	//detachShader(programId, vertexShaderId.value());
	//detachShader(programId, fragmentShaderId.value());

	//deleteShader(vertexShaderId.value());
	//deleteShader(fragmentShaderId.value());

	_programId = programId;
	return true;
}
