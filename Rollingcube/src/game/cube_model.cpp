#include "cube_model.h"

#include "utils/logger.h"
#include "utils/tangent_space.h"


namespace cubes::model
{
	namespace raw
	{
		static constexpr std::size_t verticesPerTriangle = 3;
		static constexpr std::size_t verticesPerSide = 6;
		static constexpr std::size_t trianglesPerSide = 2;
		static constexpr std::size_t verticesCount = 8;

		struct CubeVertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 tex_coord;

			constexpr CubeVertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& tex_coord) :
				position(position), normal(normal), tex_coord(tex_coord)
			{}
		};

		using CubeTriangle = unsigned int[verticesPerTriangle];


		static constexpr glm::vec3 cubeVertices[side::count][verticesPerSide] =
		{
			// Front face
			{ { side::midsize, -side::midsize, side::midsize }, { side::midsize, side::midsize, side::midsize }, { -side::midsize, side::midsize, side::midsize },
			{ -side::midsize, side::midsize, side::midsize }, { -side::midsize, -side::midsize, side::midsize }, { side::midsize, -side::midsize, side::midsize } },
			// Back face
			{ { -side::midsize, -side::midsize, -side::midsize } , { -side::midsize, side::midsize, -side::midsize }, { side::midsize, side::midsize, -side::midsize },
			{ side::midsize, side::midsize, -side::midsize }, { side::midsize, -side::midsize, -side::midsize }, { -side::midsize, -side::midsize, -side::midsize } },
			// Left face
			{ { -side::midsize, -side::midsize, side::midsize }, { -side::midsize, side::midsize, side::midsize }, { -side::midsize, side::midsize, -side::midsize },
			{ -side::midsize, side::midsize, -side::midsize }, { -side::midsize, -side::midsize, -side::midsize }, { -side::midsize, -side::midsize, side::midsize } },
			// Right face
			{ { side::midsize, -side::midsize, -side::midsize }, { side::midsize, side::midsize, -side::midsize }, { side::midsize, side::midsize, side::midsize },
			{ side::midsize, side::midsize, side::midsize }, { side::midsize, -side::midsize, side::midsize }, { side::midsize, -side::midsize, -side::midsize } },
			// Top face
			{ { side::midsize, side::midsize, side::midsize }, { side::midsize, side::midsize, -side::midsize }, { -side::midsize, side::midsize, -side::midsize },
			{ -side::midsize, side::midsize, -side::midsize }, { -side::midsize, side::midsize, side::midsize }, { side::midsize, side::midsize, side::midsize } },
			// Bottom face
			{ { side::midsize, -side::midsize, -side::midsize }, { side::midsize, -side::midsize, side::midsize }, { -side::midsize, -side::midsize, side::midsize },
			{ -side::midsize, -side::midsize, side::midsize }, { -side::midsize, -side::midsize, -side::midsize }, { side::midsize, -side::midsize, -side::midsize } },
		};

		static constexpr glm::vec2 cubeTexCoords[verticesPerSide] =
		{
			{ 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f },
			{ 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f }
		};

		static constexpr glm::vec3 cubeNormals[side::count] =
		{
			{ 0, 0, 1 },  // Front face //
			{ 0, 0, -1 }, // Back face //
			{ -1, 0, 0 }, // Left face //
			{ 1, 0, 0 },  // Right face //
			{ 0, 1, 0 },  // Top face //
			{ 0, -1, 0 }, // Bottom face //
		};

		static constexpr glm::vec3 cubeFaceColors[side::count] =
		{
			{ 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }
		};

		/*static constexpr CubeTriangle indices[cube_side_count * triangles_per_side] = {
			{ 0, 1, 2 }, { 1, 2, 3 },			// up //
			{ 4, 5, 6 }, { 5, 6, 7 },			// down //
			{ 8, 9, 10 }, { 9, 10, 11 },		// left //
			{ 12, 13, 14 }, { 13, 14, 15 },		// right //
			{ 16, 17, 18 }, { 17, 18, 19 },		// front //
			{ 20, 21, 22 }, { 21, 22, 23 },		// back //
		};*/
	}


	static bool create_mesh(Model::Ref model, side::Id sideId)
	{
		auto omesh = model->createMesh(side::name(sideId));
		if (!omesh)
			return false;

		auto& mesh = *omesh;


		// VERTEX PART //
		std::vector<glm::vec3> gl_vertices;
		std::vector<glm::vec2> gl_uvs;
		std::vector<glm::vec3> gl_normals;
		gl_vertices.resize(raw::verticesPerSide);
		gl_uvs.resize(raw::verticesPerSide);
		gl_normals.resize(raw::verticesPerSide);

		for (std::size_t i = 0; i < raw::verticesPerSide; ++i)
		{
			gl_vertices[i] = raw::cubeVertices[side::idToInt(sideId)][i];
			gl_uvs[i] = raw::cubeTexCoords[i];
			gl_normals[i] = raw::cubeNormals[side::idToInt(sideId)];
		}

		mesh.setVertices(gl_vertices);
		mesh.setUVs(gl_uvs);
		mesh.setNormals(gl_normals);


		std::vector<glm::vec3> gl_tangents;
		std::vector<glm::vec3> gl_bitangents;

		tangents::computeTangentBasis(gl_vertices, gl_uvs, gl_normals, gl_tangents, gl_bitangents);

		mesh.setTangents(gl_tangents);
		mesh.setBitangents(gl_bitangents);

		return true;
	}
	
	Model::Ref getModel()
	{
		static constinit Model::Ref CubeModel = nullptr;

		if (CubeModel == nullptr)
		{
			CubeModel = ModelManager::root().createNew(model::name.data());

			for (int i = 0; i < side::count; ++i)
			{
				if (!create_mesh(CubeModel, side::ids[i]))
					logger::fatal("CANNOT LOAD '{}' CUBE SIDE!!", side::names[i]);
			}

			CubeModel->lock();
		}

		return CubeModel;
	}
}


namespace cubes::side
{
	const glm::vec3& getDefaultMiddlePosition(side::Id sideId)
	{
		static constexpr glm::vec3 points[count] = {
			{ 0, 0, midsize },
			{ 0, 0, -midsize },
			{ -midsize, 0, 0 },
			{ midsize, 0, 0 },
			{ 0, midsize, 0 },
			{ 0, -midsize, 0 }
		};

		return points[idToInt(sideId)];
	}

	const glm::vec3& getNormal(side::Id sideId)
	{
		return model::raw::cubeNormals[idToInt(sideId)];
	}

	const glm::vec3& getRotationAngles(side::Id sideId)
	{
		static constexpr glm::vec3 points[count] = {
			{ 90, 0, 0 },
			{ -90, 0, 0 },
			{ 0, 0, -90 },
			{ 0, 0, 90 },
			{ 0, 0, 0 },
			{ 180, 0, 0 }
		};

		return points[idToInt(sideId)];
	}
}
