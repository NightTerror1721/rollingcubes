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
#include "utils/manager.h"
#include "utils/logger.h"

#include "shader.h"


class Mesh;
class Model;
class ModelManager;


class Mesh
{
private:
	using vertex_index_type = unsigned int;

	static constexpr GLuint invalid_id = 0;


	std::string _name;
	gl::VAO _vao;
	gl::EBO _ebo;
	std::vector<glm::vec3> _verticesCache;

	GLsizei _elementCount = 0;

	bool _locked = false;

public:
	Mesh() = default;
	Mesh(const Mesh&) = delete;
	Mesh& operator= (const Mesh&) = delete;

	Mesh(Mesh&& other) noexcept;
	~Mesh();

	Mesh& operator= (Mesh&& other) noexcept;


	void render(GLenum mode = GL_TRIANGLES) const;
	void render(const std::function<void(const Mesh&)>& preRenderCallback, GLenum mode = GL_TRIANGLES) const;

	void setColors(const std::vector<Color>& colors);

	inline void clear()
	{
		if (checkLocked())
			internalClear();
	}

	inline void setVertices(const std::vector<glm::vec3>& vertices)
	{
		if (checkLocked())
		{
			createVertexBuffer(constants::attributes::vertices_array_attrib_index, vertices);
			_verticesCache = vertices;
		}
	}

	inline void setUVs(const std::vector<glm::vec2>& uvs)
	{
		if (checkLocked())
			createVertexBuffer(constants::attributes::uvs_array_attrib_index, uvs);
	}

	inline void setNormals(const std::vector<glm::vec3>& normals)
	{
		if (checkLocked())
			createVertexBuffer(constants::attributes::normals_array_attrib_index, normals);
	}

	inline void setTangents(const std::vector<glm::vec3>& tangents)
	{
		if (checkLocked())
			createVertexBuffer(constants::attributes::tangents_array_attrib_index, tangents);
	}

	inline void setBitangents(const std::vector<glm::vec3>& bitangents)
	{
		if (checkLocked())
			createVertexBuffer(constants::attributes::bitangents_array_attrib_index, bitangents);
	}
	inline void setColors(const std::vector<glm::vec4>& colors)
	{
		if (checkLocked())
			createVertexBuffer(constants::attributes::colors_array_attrib_index, colors);
	}

	inline void setElements(const std::vector<vertex_index_type>& elements)
	{
		if (checkLocked())
		{
			createVertexArray(elements);
			_elementCount = GLsizei(elements.size());
		}
	}

	inline void setName(const std::string_view& name) { if (checkLocked()) _name = name; }
	inline const std::string& getName() const { return _name; }


	inline const std::vector<glm::vec3>& getVertices() const { return _verticesCache; }

	constexpr void lock() { _locked = true; }
	constexpr bool isLocked() const { return _locked; }

private:
	void internalClear();

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

	inline bool checkLocked() const
	{
		if (isLocked())
		{
			logger::warn("Attempt to modify locked Mesh {}.", getName());
			return false;
		}

		return true;
	}

public:
	friend Model;
};



class Model
{
public:
	using Ref = ManagerReference<Model>;
	using ConstRef = ConstManagerReference<Model>;

	using iterator = std::vector<std::shared_ptr<Mesh>>::iterator;
	using const_iterator = std::vector<std::shared_ptr<Mesh>>::const_iterator;

private:
	std::vector<std::shared_ptr<Mesh>> _meshes;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> _meshesMap;
	bool _locked = false;

public:
	Model() = default;
	Model(const Model&) = delete;
	Model(Model&&) = default;
	~Model() noexcept = default;

	Model& operator= (const Model&) = delete;
	Model& operator= (Model&&) noexcept = default;

public:
	optref<Mesh> createMesh(const std::string_view& name);

	optref<Mesh> safeGetMesh(std::size_t index);
	const_optref<Mesh> safeGetMesh(std::size_t index) const;

	optref<Mesh> safeGetMesh(const std::string_view& name);
	const_optref<Mesh> safeGetMesh(const std::string_view& name) const;

	void render(GLenum mode = GL_TRIANGLES) const;
	void render(const std::function<void(const Mesh&)>& preMeshRenderCallback, GLenum mode = GL_TRIANGLES) const;

	bool load(const std::string_view& filename, bool computeTangentBasis);

	inline void clear()
	{
		if (checkLocked())
			internalClear();
	}
	
	inline Mesh& getMesh(std::size_t index) { return *safeGetMesh(index); }
	inline const Mesh& getMesh(std::size_t index) const { return *safeGetMesh(index); }

	inline Mesh& getMesh(const std::string_view& name) { return *safeGetMesh(name); }
	inline const Mesh& getMesh(const std::string_view& name) const { return *safeGetMesh(name); }

	constexpr void lock() { _locked = true; }
	constexpr bool isLocked() const { return _locked; }

public:
	inline iterator begin() noexcept { return _meshes.begin(); }
	inline const_iterator begin() const noexcept { return _meshes.begin(); }
	inline const_iterator cbegin() const noexcept { return _meshes.cbegin(); }
	inline iterator end() noexcept { return _meshes.end(); }
	inline const_iterator end() const noexcept { return _meshes.end(); }
	inline const_iterator cend() const noexcept { return _meshes.cend(); }

private:
	void internalClear();

	inline bool checkLocked() const
	{
		if (isLocked())
		{
			logger::warn("Attempt to modify locked Model.");
			return false;
		}

		return true;
	}
};





class ModelManager : public Manager<Model>
{
private:
	static ModelManager Root;

public:
	Reference loadFromFile(std::string_view filename, bool computeTangentBasis);

	Reference createNew(const IdType& id);

public:
	static inline ModelManager& root() { return Root; }

	inline ModelManager createChildManager() { return ModelManager(this); }

private:
	inline explicit ModelManager(Manager<Model>* parent) :
		Manager(parent)
	{}
};
