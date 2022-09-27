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

void Mesh::render(GLenum mode)
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


void ObjModel::render(GLenum mode)
{
	std::size_t const len = _meshes.size();
	const auto* ptr = _meshes.data();
	for (std::size_t i = 0; i < len; ++i)
		ptr[i]->render(mode);
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












OldMesh::~OldMesh()
{
	destroyVRAMData();
}

void OldMesh::clear()
{
	_rebuildBB = true;
	_reloadToVRam = true;
	destroyVRAMData();

	_vertices.clear();
	_normals.clear();
	_uvs.clear();
	_colors.clear();
	_bb.clear();
}

void OldMesh::render(int primitive) const
{
	assert(!_vertices.empty() && "No vertices in this OldMesh");

	createVRAMData();

	if (!_vertices.empty())
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		if (_verticesVBOId)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _verticesVBOId);
			glVertexPointer(3, GL_FLOAT, 0, NULL);
		}
		else
			glVertexPointer(3, GL_FLOAT, 0, &*_vertices.data());
	}

	if (!_normals.empty())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		if (_normalsVBOId)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _normalsVBOId);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}
		else
			glNormalPointer(GL_FLOAT, 0, &*_normals.data());
	}

	if (!_uvs.empty())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if (_uvsVBOId)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _uvsVBOId);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}
		else
			glTexCoordPointer(2, GL_FLOAT, 0, &*_uvs.data());
	}

	if (!_colors.empty())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		if (_colorsVBOId)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _colorsVBOId);
			glColorPointer(4, GL_FLOAT, 0, NULL);
		}
		else
			glColorPointer(4, GL_FLOAT, 0, &*_colors.data());
	}

	glDrawArrays(primitive, 0, (GLsizei)_vertices.size());

	if (!_normals.empty())
		glDisableClientState(GL_VERTEX_ARRAY);

	if (!_normals.empty())
		glDisableClientState(GL_NORMAL_ARRAY);

	if (!_uvs.empty())
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (!_colors.empty())
		glDisableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//	void OldMesh::render(int primitive, const Shader& shader) const;
void OldMesh::renderBoundingBox(int primitive, const matrix44f& model, Color color) const
{
	createBoundingBoxOldMesh();
	if (!_bb.empty())
	{
		glPushMatrix();
		model.gl_mult();
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &*_bb.data());
		color.gl_color<false>();
		glDrawArrays(GL_LINES, 0, GLsizei(_bb.size()));
		glDisableClientState(GL_VERTEX_ARRAY);
		glPopMatrix();
	}
}

void OldMesh::createPlane(float size)
{
	clear();

	_vertices.push_back(vector3f(size, size, 0));
	_vertices.push_back(vector3f(size, -size, 0));
	_vertices.push_back(vector3f(-size, -size, 0));
	_vertices.push_back(vector3f(-size, size, 0));
	_vertices.push_back(vector3f(size, size, 0));
	_vertices.push_back(vector3f(-size, -size, 0));

	//all of them have the same normal
	_normals.push_back(vector3f(0, 0, 1));
	_normals.push_back(vector3f(0, 0, 1));
	_normals.push_back(vector3f(0, 0, 1));
	_normals.push_back(vector3f(0, 0, 1));
	_normals.push_back(vector3f(0, 0, 1));
	_normals.push_back(vector3f(0, 0, 1));

	//texture coordinates
	_uvs.push_back(vector2f(1, 1));
	_uvs.push_back(vector2f(1, 0));
	_uvs.push_back(vector2f(0, 0));
	_uvs.push_back(vector2f(0, 1));
	_uvs.push_back(vector2f(1, 1));
	_uvs.push_back(vector2f(0, 0));
}

void OldMesh::createQuad(float center_x, float center_y, float w, float h, bool flip_uvs)
{
	clear();

	_vertices.push_back(vector3f(center_x + w * 0.5f, center_y + h * 0.5f, 0.0f));
	_vertices.push_back(vector3f(center_x - w * 0.5f, center_y - h * 0.5f, 0.0f));
	_vertices.push_back(vector3f(center_x + w * 0.5f, center_y - h * 0.5f, 0.0f));
	_vertices.push_back(vector3f(center_x - w * 0.5f, center_y + h * 0.5f, 0.0f));
	_vertices.push_back(vector3f(center_x - w * 0.5f, center_y - h * 0.5f, 0.0f));
	_vertices.push_back(vector3f(center_x + w * 0.5f, center_y + h * 0.5f, 0.0f));

	//texture coordinates
	_uvs.push_back(vector2f(1.0f, flip_uvs ? 0.0f : 1.0f));
	_uvs.push_back(vector2f(0.0f, flip_uvs ? 1.0f : 0.0f));
	_uvs.push_back(vector2f(1.0f, flip_uvs ? 1.0f : 0.0f));
	_uvs.push_back(vector2f(0.0f, flip_uvs ? 0.0f : 1.0f));
	_uvs.push_back(vector2f(0.0f, flip_uvs ? 1.0f : 0.0f));
	_uvs.push_back(vector2f(1.0f, flip_uvs ? 0.0f : 1.0f));

	//all of them have the same normal
	_normals.push_back(vector3f(0.0f, 0.0f, 1.0f));
	_normals.push_back(vector3f(0.0f, 0.0f, 1.0f));
	_normals.push_back(vector3f(0.0f, 0.0f, 1.0f));
	_normals.push_back(vector3f(0.0f, 0.0f, 1.0f));
	_normals.push_back(vector3f(0.0f, 0.0f, 1.0f));
	_normals.push_back(vector3f(0.0f, 0.0f, 1.0f));
}

void OldMesh::createBIN(const std::string_view& filename) const
{
	std::string bfilename = std::string(filename) + ".BIN";

	std::ofstream bfile(bfilename, std::ios::out | std::ios::binary);

	io::write_obj(bfile, { std::uint64_t(_vertices.size()), std::uint64_t(_uvs.size()), std::uint64_t(_normals.size()) });
	io::write_obj(bfile, _vertices.data(), _vertices.size());
	io::write_obj(bfile, _uvs.data(), _uvs.size());
	io::write_obj(bfile, _normals.data(), _normals.size());
	io::write_obj(bfile, &_info);

	bfile.close();
}

bool OldMesh::loadBIN(const std::string_view& filename)
{
	std::string bfilename = std::string(filename) + ".BIN";

	std::ifstream bfile(bfilename, std::ios::in | std::ios::binary);
	if (!bfile)
		return false;

	std::uint64_t sizes[3];
	io::read_obj(bfile, sizes, 3);

	_vertices.resize(sizes[0]);
	_uvs.resize(sizes[1]);
	_normals.resize(sizes[2]);

	io::read_obj(bfile, _vertices.data(), sizes[0]);
	io::read_obj(bfile, _uvs.data(), sizes[1]);
	io::read_obj(bfile, _normals.data(), sizes[2]);
	io::read_obj(bfile, &_info);

	bfile.close();

	return true;
}

//bool OldMesh::loadOldMesh(const std::string_view& filename);

void OldMesh::createVRAMData() const
{
	if (!_usingVBO)
		return;

	if (!_reloadToVRam)
		return;

	destroyVRAMData();

	createVBO(_verticesVBOId, _vertices);
	createVBO(_normalsVBOId, _normals);
	createVBO(_uvsVBOId, _uvs);
	createVBO(_colorsVBOId, _colors);

	_reloadToVRam = false;
}

void OldMesh::destroyVRAMData() const
{
	destroyVBO(_verticesVBOId);
	destroyVBO(_normalsVBOId);
	destroyVBO(_uvsVBOId);
	destroyVBO(_colorsVBOId);
}

void OldMesh::createBoundingBoxOldMesh() const
{
	if (!_rebuildBB)
		return;

	vector3f box[8]{
			vector3f(_info.center.x - _info.halfsize.x, _info.center.y + _info.halfsize.y, _info.center.z - _info.halfsize.z),
			vector3f(_info.center.x + _info.halfsize.x, _info.center.y + _info.halfsize.y, _info.center.z - _info.halfsize.z),
			vector3f(_info.center.x + _info.halfsize.x, _info.center.y + _info.halfsize.y, _info.center.z + _info.halfsize.z),
			vector3f(_info.center.x - _info.halfsize.x, _info.center.y + _info.halfsize.y, _info.center.z + _info.halfsize.z),
			vector3f(_info.center.x - _info.halfsize.x, _info.center.y - _info.halfsize.y, _info.center.z - _info.halfsize.z),
			vector3f(_info.center.x + _info.halfsize.x, _info.center.y - _info.halfsize.y, _info.center.z - _info.halfsize.z),
			vector3f(_info.center.x + _info.halfsize.x, _info.center.y - _info.halfsize.y, _info.center.z + _info.halfsize.z),
			vector3f(_info.center.x - _info.halfsize.x, _info.center.y - _info.halfsize.y, _info.center.z + _info.halfsize.z)
	};

	_bb.resize(24);

	_bb[0] = box[0];
	_bb[1] = box[1];
	_bb[2] = box[1];
	_bb[3] = box[2];
	_bb[4] = box[2];
	_bb[5] = box[3];
	_bb[6] = box[3];
	_bb[7] = box[0];
	_bb[8] = box[4];
	_bb[9] = box[5];
	_bb[10] = box[5];
	_bb[11] = box[6];
	_bb[12] = box[6];
	_bb[13] = box[7];
	_bb[14] = box[7];
	_bb[15] = box[4];
	_bb[16] = box[0];
	_bb[17] = box[4];
	_bb[18] = box[1];
	_bb[19] = box[5];
	_bb[20] = box[2];
	_bb[21] = box[6];
	_bb[22] = box[3];
	_bb[23] = box[7];

	_rebuildBB = true;
}

void OldMesh::notifyChanges()
{
	_reloadToVRam = true;
	_rebuildBB = true;
}
