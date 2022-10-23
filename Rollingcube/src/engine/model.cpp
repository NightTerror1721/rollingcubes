#include "model.h"

#include <cassert>

#pragma warning(push)
#pragma warning(disable: 26451)
#include <Bly7/OBJ_loader.hpp>
#pragma warning(pop)

#include "utils/io_utils.h"
#include "utils/tangent_space.h"


Mesh::Mesh(Mesh&& other) noexcept :
	_vao(std::move(other._vao)),
	_elementCount(other._elementCount)
{
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

void Mesh::internalClear()
{
	_verticesCache.clear();

	_vao.destroy();
	_elementCount = 0;
}

void Mesh::render(GLenum mode) const
{
	if (_ebo.isCreated())
		gl::render(_vao, _ebo, mode);
	else
		gl::render(_vao, mode);
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







void Model::internalClear()
{
	_meshes.clear();
	_meshesMap.clear();
}

optref<Mesh> Model::createMesh(const std::string_view& sv_name)
{
	if (!checkLocked())
		return optref<Mesh>::empty();

	auto name = std::string(sv_name);
	if (_meshesMap.contains(name))
		return optref<Mesh>::empty();

	auto ptr = std::make_shared<Mesh>();
	_meshes.push_back(ptr);
	_meshesMap.emplace(name, ptr);
	return optref<Mesh>::of(ptr.get());
}

optref<Mesh> Model::safeGetMesh(std::size_t index)
{
	if(index >= _meshes.size())
		return optref<Mesh>::empty();
	return optref<Mesh>::of(_meshes[index].get());
}

const_optref<Mesh> Model::safeGetMesh(std::size_t index) const
{
	if (index >= _meshes.size())
		return const_optref<Mesh>::empty();
	return const_optref<Mesh>::of(_meshes[index].get());
}

optref<Mesh> Model::safeGetMesh(const std::string_view& name)
{
	decltype(auto) it = _meshesMap.find(std::string(name));
	if(it == _meshesMap.end())
		return optref<Mesh>::empty();
	return optref<Mesh>::of(it->second.get());
}

const_optref<Mesh> Model::safeGetMesh(const std::string_view& name) const
{
	decltype(auto) it = _meshesMap.find(std::string(name));
	if (it == _meshesMap.end())
		return const_optref<Mesh>::empty();
	return const_optref<Mesh>::of(it->second.get());
}


void Model::render(GLenum mode) const
{
	std::size_t const len = _meshes.size();
	const auto* ptr = _meshes.data();
	for (std::size_t i = 0; i < len; ++i)
		ptr[i]->render(mode);
}

void Model::render(const std::function<void(const Mesh&)>& preMeshRenderCallback, GLenum mode) const
{
	std::size_t const len = _meshes.size();
	const auto* ptr = _meshes.data();
	for (std::size_t i = 0; i < len; ++i)
		ptr[i]->render(preMeshRenderCallback, mode);
}



using loader_vertex_index_t = decltype(std::declval<objl::Mesh>().Indices)::value_type;
static bool loader_newMesh(
	Model& model,
	const std::string& name,
	const std::vector<objl::Vertex>& vertices,
	const std::vector<loader_vertex_index_t>& indices,
	bool computeTangentBasis)
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


	// TANGENT PART //
	if (computeTangentBasis)
	{
		std::vector<glm::vec3> gl_tangents;
		std::vector<glm::vec3> gl_bitangents;

		tangents::computeTangentBasis(gl_vertices, gl_uvs, gl_normals, gl_tangents, gl_bitangents);

		mesh.setTangents(gl_tangents);
		mesh.setBitangents(gl_bitangents);
	}


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

bool Model::load(const std::string_view& filename, bool computeTangentBasis)
{
	if (!checkLocked())
		return false;

	internalClear();

	objl::Loader loader;
	if (!loader.LoadFile(std::string(filename)))
	{
		std::cerr << "Loading OBJ error in file: " << filename << std::endl;
		return false;
	}

	if (loader.LoadedMeshes.empty())
	{
		if (!loader.LoadedVertices.empty())
			return loader_newMesh(*this, "default", loader.LoadedVertices, loader.LoadedIndices, computeTangentBasis);
	}
	else
	{
		for (const auto& mesh : loader.LoadedMeshes)
		{
			if (!loader_newMesh(*this, mesh.MeshName, mesh.Vertices, mesh.Indices, computeTangentBasis))
				std::cerr << "Loading OBJ error. Cannot load " << mesh.MeshName << " mesh. File: " << filename << std::endl;
		}
	}

	return true;
}






ModelManager ModelManager::Root = ModelManager(nullptr);

ModelManager::Reference ModelManager::loadFromFile(std::string_view filename, bool computeTangentBasis)
{
	Reference ref = get(filename.data());
	if (ref != nullptr)
		return ref;

	ref = emplace(filename.data());
	if (!ref)
		return nullptr;

	if (!ref->load(filename, computeTangentBasis))
	{
		destroy(filename.data());
		return nullptr;
	}

	ref->lock();

	return ref;
}

ModelManager::Reference ModelManager::createNew(const IdType& id)
{
	if (contains(id))
	{
		logger::error("Attempt to create new Model with already exists ID {}.", id);
		return nullptr;
	}

	return emplace(id);
}
