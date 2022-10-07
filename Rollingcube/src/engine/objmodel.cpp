#include "objmodel.h"

#include <cassert>

#include <Bly7/OBJ_loader.hpp>

#include "utils/io_utils.h"


Mesh::Mesh(Mesh&& other) noexcept :
	_vaoId(other._vaoId),
	_verticesVboId(other._verticesVboId),
	_uvsVboId(other._uvsVboId),
	_normalsVboId(other._normalsVboId),
	_tangentsVboId(other._tangentsVboId),
	_bitangentVboId(other._bitangentVboId),
	_colorsVboId(other._colorsVboId),
	_elementsIboId(other._elementsIboId),
	_verticesCount(other._verticesCount),
	_elementCount(other._elementCount)
{
	other._vaoId = invalid_id;
	other._verticesVboId = invalid_id;
	other._uvsVboId = invalid_id;
	other._normalsVboId = invalid_id;
	other._tangentsVboId = invalid_id;
	other._bitangentVboId = invalid_id;
	other._colorsVboId = invalid_id;
	other._elementsIboId = invalid_id;
	other._verticesCount = 0;
	other._elementCount = 0;
}

Mesh::~Mesh()
{
	clear();
}

Mesh& Mesh::operator= (Mesh&& other) noexcept
{
	std::destroy_at(this);
	return *std::construct_at<Mesh, Mesh&&>(this, std::move(other));
}

void Mesh::clear()
{
	destroyBuffer(_verticesVboId);
	destroyBuffer(_uvsVboId);
	destroyBuffer(_normalsVboId);
	destroyBuffer(_tangentsVboId);
	destroyBuffer(_bitangentVboId);
	destroyBuffer(_colorsVboId);
	destroyBuffer(_elementsIboId);
	_verticesCount = 0;
	_elementCount = 0;
}

void Mesh::render(GLenum mode) const
{
	if (_vaoId != invalid_id)
	{
		glBindVertexArray(_vaoId);

		if (_elementCount > 0)
		{
			glDrawElements(mode, _elementCount, GL_UNSIGNED_INT, nullptr);
		}
		else if (_verticesCount > 0)
		{
			glDrawArrays(mode, 0, _verticesCount);
		}

		glBindVertexArray(invalid_id);
	}
}

void Mesh::render(const std::function<void(const Mesh&)>& preRenderCallback, GLenum mode) const
{
	preRenderCallback(*this);
	render(mode);
}

void Mesh::setColors(const std::vector<Color>& colors)
{
	std::vector<glm::vec4> vcolors;
	vcolors.reserve(colors.size());
	for (const auto& color : colors)
		vcolors.push_back(glm::vec4(color));
	setColors(vcolors);
}


void Mesh::destroyBuffer(GLuint& bufferId)
{
	if (bufferId != invalid_id)
	{
		glDeleteBuffers(1, &bufferId);
		bufferId = invalid_id;
	}
}

void Mesh::createBuffer(GLuint& bufferId, const std::vector<vertex_index_type>& vector)
{
	destroyBuffer(bufferId);
	if (!vector.empty())
	{
		enableVao();
		glGenBuffers(1, &bufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_index_type) * vector.size(), vector.data(), GL_STATIC_DRAW);
		disableVao();
	}
}







void ObjModel::clear()
{
	_meshes.clear();
	_meshesMap.clear();
}

optref<Mesh> ObjModel::createMesh(const std::string_view& sv_name)
{
	auto name = std::string(sv_name);
	if (_meshesMap.contains(name))
		return optref<Mesh>::empty();

	auto ptr = std::make_shared<Mesh>();
	_meshes.push_back(ptr);
	_meshesMap.emplace(name, ptr);
	return optref<Mesh>::of(ptr.get());
}

optref<Mesh> ObjModel::safeGetMesh(std::size_t index)
{
	if(index >= _meshes.size())
		return optref<Mesh>::empty();
	return optref<Mesh>::of(_meshes[index].get());
}

const_optref<Mesh> ObjModel::safeGetMesh(std::size_t index) const
{
	if (index >= _meshes.size())
		return const_optref<Mesh>::empty();
	return const_optref<Mesh>::of(_meshes[index].get());
}

optref<Mesh> ObjModel::safeGetMesh(const std::string_view& name)
{
	decltype(auto) it = _meshesMap.find(std::string(name));
	if(it == _meshesMap.end())
		return optref<Mesh>::empty();
	return optref<Mesh>::of(it->second.get());
}

const_optref<Mesh> ObjModel::safeGetMesh(const std::string_view& name) const
{
	decltype(auto) it = _meshesMap.find(std::string(name));
	if (it == _meshesMap.end())
		return const_optref<Mesh>::empty();
	return const_optref<Mesh>::of(it->second.get());
}


void ObjModel::render(GLenum mode) const
{
	std::size_t const len = _meshes.size();
	const auto* ptr = _meshes.data();
	for (std::size_t i = 0; i < len; ++i)
		ptr[i]->render(mode);
}

void ObjModel::render(const std::function<void(const Mesh&)>& preMeshRenderCallback, GLenum mode) const
{
	std::size_t const len = _meshes.size();
	const auto* ptr = _meshes.data();
	for (std::size_t i = 0; i < len; ++i)
		ptr[i]->render(preMeshRenderCallback, mode);
}



using loader_vertex_index_t = decltype(std::declval<objl::Mesh>().Indices)::value_type;
static bool loader_newMesh(
	ObjModel& model,
	const std::string& name,
	const std::vector<objl::Vertex>& vertices,
	const std::vector<loader_vertex_index_t>& indices)
{
	auto omesh = model.createMesh(name);
	if (!omesh)
		return false;

	auto& mesh = *omesh;


	// VERTEX PART //
	std::vector<glm::vec3> gl_vertices;
	std::vector<glm::vec2> gl_uvs;
	std::vector<glm::vec3> gl_normals;
	gl_vertices.resize(vertices.size());
	gl_uvs.resize(vertices.size());
	gl_normals.resize(vertices.size());

	for (std::size_t i = 0; i < vertices.size(); ++i)
	{
		const auto& vertex = vertices[i];
		gl_vertices[i] = { vertex.Position.X, vertex.Position.Y, vertex.Position.Z };
		gl_uvs[i] = { vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y };
		gl_normals[i] = { vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z };
	}

	mesh.setVertices(gl_vertices);
	mesh.setUVs(gl_uvs);
	mesh.setNormals(gl_normals);


	// INDEX PART //
	std::vector<GLuint> gl_indices;
	if constexpr (std::same_as<GLuint, loader_vertex_index_t>)
		mesh.setElements(indices);
	else
	{
		std::vector<GLuint> gl_indices;
		gl_indices.resize(indices.size());

		for (std::size_t i = 0; i < indices.size(); ++i)
		{
			const auto& index = indices[i];
			gl_indices[i] = GLuint(indices[i]);
		}
		mesh.setElements(gl_indices);
	}

	return true;
}

bool ObjModel::load(const std::string_view& filename)
{
	clear();

	objl::Loader loader;
	if (!loader.LoadFile(std::string(filename)))
	{
		std::cerr << "Loading OBJ error in file: " << filename << std::endl;
		return false;
	}

	if (loader.LoadedMeshes.empty())
	{
		if (!loader.LoadedVertices.empty())
			return loader_newMesh(*this, "default", loader.LoadedVertices, loader.LoadedIndices);
	}
	else
	{
		for (const auto& mesh : loader.LoadedMeshes)
		{
			if (!loader_newMesh(*this, mesh.MeshName, mesh.Vertices, mesh.Indices))
				std::cerr << "Loading OBJ error. Cannot load " << mesh.MeshName << " mesh. File: " << filename << std::endl;
		}
	}

	return true;
}
