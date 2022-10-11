#pragma once

#include <compare>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

#include "core/gl.h"
#include "core/render.h"
#include "math/glm.h"
#include "math/color.h"
#include "utils/optref.h"
#include "utils/shader_constants.h"

#include "shader.h"


class Mesh
{
private:
	using vertex_index_type = unsigned int;

	static constexpr GLuint invalid_id = 0;


	std::string _name;

	gl::VAO _vao;
	gl::EBO _ebo;


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

	void render(GLenum mode = GL_TRIANGLES) const;
	void render(const std::function<void(const Mesh&)>& preRenderCallback, GLenum mode = GL_TRIANGLES) const;

	void setColors(const std::vector<Color>& colors);

	inline void setVertices(const std::vector<glm::vec3>& vertices)
	{
		createVertexBuffer(constants::attributes::vertices_array_attrib_index, vertices);
		_verticesCount = GLsizei(vertices.size());
	}
	inline void setUVs(const std::vector<glm::vec2>& uvs) { createVertexBuffer(constants::attributes::uvs_array_attrib_index, uvs); }
	inline void setNormals(const std::vector<glm::vec3>& normals) { createVertexBuffer(constants::attributes::normals_array_attrib_index, normals); }
	inline void setTangents(const std::vector<glm::vec3>& tangents) { createVertexBuffer(constants::attributes::tangents_array_attrib_index, tangents); }
	inline void setBitangents(const std::vector<glm::vec3>& bitangents)
	{
		createVertexBuffer(constants::attributes::bitangents_array_attrib_index, bitangents);
	}
	inline void setColors(const std::vector<glm::vec4>& colors) { createVertexBuffer(constants::attributes::colors_array_attrib_index, colors); }
	inline void setElements(const std::vector<vertex_index_type>& elements)
	{
		createVertexArray(elements);
		_elementCount = GLsizei(elements.size());
	}

	inline void setName(const std::string_view& name) { _name = name; }
	inline const std::string& getName() const { return _name; }

private:
	template <typename _Ty> requires
		std::same_as<_Ty, glm::vec2> || std::same_as<_Ty, glm::vec3> || std::same_as<_Ty, glm::vec4>
	inline void createVertexBuffer(GLuint shaderAttribIndex, const std::vector<_Ty>&vector)
	{
		_vao.createAttribute(
			shaderAttribIndex,
			gl::to_component_count(_Ty::length()),
			gl::DataType::Float,
			GL_FALSE,
			0,
			vector,
			gl::VBO::Usage::StaticDraw
		);
	}

	inline void createVertexArray(const std::vector<vertex_index_type>& vector)
	{
		_ebo.write(vector, gl::EBO::Usage::StaticDraw);
	}
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


	void render(GLenum mode = GL_TRIANGLES) const;
	void render(const std::function<void(const Mesh&)>& preMeshRenderCallback, GLenum mode = GL_TRIANGLES) const;


	bool load(const std::string_view& filename);
	


	inline Mesh& getMesh(std::size_t index) { return *safeGetMesh(index); }
	inline const Mesh& getMesh(std::size_t index) const { return *safeGetMesh(index); }

	inline Mesh& getMesh(const std::string_view& name) { return *safeGetMesh(name); }
	inline const Mesh& getMesh(const std::string_view& name) const { return *safeGetMesh(name); }
};
