#pragma once

#include <compare>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "core/gl.h"
#include "math/glm.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix44.h"
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





struct OldMeshInfo
{
	vector3f min_aabb;
	vector3f max_aabb;
	vector3f center;
	vector3f halfsize;
	float length;
};



class OldMesh
{
public:
	using Info = OldMeshInfo;

private:
	mutable unsigned int _verticesVBOId = 0;
	mutable unsigned int _uvsVBOId = 0;
	mutable unsigned int _normalsVBOId = 0;
	mutable unsigned int _colorsVBOId = 0;
	mutable bool _reloadToVRam = true;
	mutable bool _rebuildBB = true;
	bool _usingVBO = true;

	Info _info;

	std::vector<vector3f> _vertices;
	std::vector<vector3f> _normals;
	std::vector<vector2f> _uvs;
	std::vector<vector4f> _colors;
	mutable std::vector<vector3f> _bb;

public:
	OldMesh() = default;
	OldMesh(const OldMesh&) = default;
	OldMesh(OldMesh&&) noexcept = default;

	OldMesh& operator= (const OldMesh&) = default;
	OldMesh& operator= (OldMesh&&) noexcept = default;

	bool operator== (const OldMesh&) const = default;


	~OldMesh();

	void clear();

	void render(int primitive) const;
//	void render(int primitive, const Shader& shader) const;
	void renderBoundingBox(int primitive, const matrix44f& model, Color color = Color::green()) const;

	void createPlane(float size);
	void createQuad(float center_x, float center_y, float w, float h, bool flip_uvs);

	void createBIN(const std::string_view& filename) const;

	bool loadBIN(const std::string_view& filename);
//	bool loadOldMesh(const std::string_view& filename);


	inline void setVBOMolde(bool flag) { _usingVBO = flag; }

private:
	void createVRAMData() const;
	void destroyVRAMData() const;
	void createBoundingBoxOldMesh() const;
	void notifyChanges();

	static inline void destroyVBO(unsigned int& VBO_id) { glDeleteBuffers(1, &VBO_id); }

	template <typename _Ty> requires std::same_as<_Ty, vector2f> || std::same_as<_Ty, vector3f> || std::same_as<_Ty, vector4f>
	static inline void createVBO(unsigned int& VBO_id, const std::vector<_Ty>&vector)
	{
		if (!vector.empty())
		{
			glGenBuffers(1, &VBO_id); //generate one handler
			glBindBuffer(GL_ARRAY_BUFFER, VBO_id); //bind the handler
			glBufferData(GL_ARRAY_BUFFER, vector.size() * _Ty::dimension_count * sizeof(_Ty::value_type), &*vector.data(), GL_STATIC_DRAW); //upload data
		}
	}
};
