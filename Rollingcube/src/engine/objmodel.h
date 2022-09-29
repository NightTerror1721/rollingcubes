#pragma once

#include <compare>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "core/gl.h"
#include "math/glm.h"
#include "math/color.h"
#include "utils/optref.h"

#include "shader.h"


class Mesh
{
private:
	using vertex_index_type = unsigned int;

	static constexpr GLuint invalid_id = 0;

	std::string _name;

	GLuint _vaoId = invalid_id;
	
	GLuint _verticesVboId = invalid_id;
	GLuint _uvsVboId = invalid_id;
	GLuint _normalsVboId = invalid_id;
	GLuint _tangentsVboId = invalid_id;
	GLuint _bitangentVboId = invalid_id;
	GLuint _colorsVboId = invalid_id;
	GLuint _elementsIboId = invalid_id;

	GLsizei _verticesCount = 0;
	GLsizei _elementCount = 0;

public:
	Mesh() = default;
	Mesh(const Mesh&) = delete;
	Mesh& operator= (const Mesh&) = delete;

	Mesh(Mesh&& other) noexcept;
	~Mesh();

	Mesh& operator= (Mesh&& other) noexcept;


	void clear();

	void render(GLenum mode = GL_TRIANGLES);

	void setColors(const std::vector<Color>& colors);

	inline void setVertices(const std::vector<glm::vec3>& vertices)
	{
		createBuffer(_verticesVboId, Shader::vertices_array_attrib_index, vertices);
		_verticesCount = GLsizei(vertices.size());
	}
	inline void setUVs(const std::vector<glm::vec2>& uvs) { createBuffer(_uvsVboId, Shader::uvs_array_attrib_index, uvs); }
	inline void setNormals(const std::vector<glm::vec3>& normals) { createBuffer(_normalsVboId, Shader::normals_array_attrib_index, normals); }
	inline void setTangents(const std::vector<glm::vec3>& tangents) { createBuffer(_tangentsVboId, Shader::tangents_array_attrib_index, tangents); }
	inline void setBitangents(const std::vector<glm::vec3>& bitangents)
	{
		createBuffer(_bitangentVboId, Shader::bitangents_array_attrib_index, bitangents);
	}
	inline void setColors(const std::vector<glm::vec4>& colors) { createBuffer(_colorsVboId, Shader::colors_array_attrib_index, colors); }
	inline void setElements(const std::vector<vertex_index_type>& elements)
	{
		createBuffer(_elementsIboId, elements);
		_elementCount = GLsizei(elements.size());
	}

	inline void setName(const std::string_view& name) { _name = name; }
	inline const std::string& getName() const { return _name; }

private:
	static void destroyBuffer(GLuint& bufferId);

	inline void enableVao()
	{
		if (_vaoId == invalid_id)
			glGenVertexArrays(1, &_vaoId);

		glBindVertexArray(_vaoId);
	}

	static inline void disableVao()
	{
		glBindVertexArray(invalid_id);
	}

	template <typename _Ty> requires
		std::same_as<_Ty, glm::vec2> || std::same_as<_Ty, glm::vec3> || std::same_as<_Ty, glm::vec4>
	void createBuffer(GLuint& bufferId, GLuint shaderAttribIndex, const std::vector<_Ty>&vector)
	{
		enableVao();
		destroyBuffer(bufferId);
		if (!vector.empty())
		{
			glGenBuffers(1, &bufferId);
			glBindBuffer(GL_ARRAY_BUFFER, bufferId);
			glBufferData(GL_ARRAY_BUFFER, sizeof(_Ty) * vector.size(), &vector, GL_STATIC_DRAW);
			glVertexAttribPointer(shaderAttribIndex, _Ty::length(), GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(shaderAttribIndex);
		}
		disableVao();
	}

	void createBuffer(GLuint& bufferId, const std::vector<vertex_index_type>& vector);
};



class ObjModel
{
private:
	std::vector<std::shared_ptr<Mesh>> _meshes;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> _meshesMap;

public:
	ObjModel() = default;
	ObjModel(const ObjModel&) = delete;
	ObjModel(ObjModel&&) = default;
	~ObjModel() noexcept = default;

	ObjModel& operator= (const ObjModel&) = delete;
	ObjModel& operator= (ObjModel&&) noexcept = default;


	void clear();

	optref<Mesh> createMesh(const std::string_view& name);

	optref<Mesh> safeGetMesh(std::size_t index);
	const_optref<Mesh> safeGetMesh(std::size_t index) const;

	optref<Mesh> safeGetMesh(const std::string_view& name);
	const_optref<Mesh> safeGetMesh(const std::string_view& name) const;


	void render(GLenum mode = GL_TRIANGLES);


	bool load(const std::string_view& filename);
	


	inline Mesh& getMesh(std::size_t index) { return *safeGetMesh(index); }
	inline const Mesh& getMesh(std::size_t index) const { return *safeGetMesh(index); }

	inline Mesh& getMesh(const std::string_view& name) { return *safeGetMesh(name); }
	inline const Mesh& getMesh(const std::string_view& name) const { return *safeGetMesh(name); }
};
