#include "cube_model.h"

#include "utils/logger.h"
#include "utils/tangent_space.h"


namespace cubes::model
{
	static std::shared_ptr<ObjModel> CubeModel = nullptr;

	namespace raw
	{
		static constexpr std::size_t vertices_per_triangle = 3;
		static constexpr std::size_t vertices_per_side = 4;
		static constexpr std::size_t triangles_per_side = 2;
		static constexpr std::size_t vertices_count = 8;

		struct CubeVertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 tex_coord;

			constexpr CubeVertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& tex_coord) :
				position(position), normal(normal), tex_coord(tex_coord)
			{}
		};

		using CubeTriangle = unsigned int[vertices_per_triangle];


		static constexpr glm::vec3 cubeVertices[36] =
		{
			// Front face
			{ 0.5f, -0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f },
			// Back face
			{ -0.5f, -0.5f, -0.5f } , { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f },
			// Left face
			{ -0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, 0.5f },
			// Right face
			{ 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, -0.5f },
			// Top face
			{ 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f },
			// Bottom face
			{ 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f },
		};

		static constexpr glm::vec2 cubeTexCoords[6] =
		{
			{ 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f },
			{ 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f }
		};

		static constexpr glm::vec3 cubeNormals[6] =
		{
			{ 0, 0, 1 },  // Front face //
			{ 0, 0, -1 }, // Back face //
			{ -1, 0, 0 }, // Left face //
			{ 1, 0, 0 },  // Right face //
			{ 0, 1, 0 },  // Top face //
			{ 0, -1, 0 }, // Bottom face //
		};

		static constexpr glm::vec3 cubeFaceColors[6] =
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


	static bool create_mesh(ObjModel& model, unsigned int side_id)
	{
		auto omesh = model.createMesh(cube_side_name[side_id]);
		if (!omesh)
			return false;

		auto& mesh = *omesh;


		// VERTEX PART //
		std::vector<glm::vec3> gl_vertices;
		std::vector<glm::vec2> gl_uvs;
		std::vector<glm::vec3> gl_normals;
		gl_vertices.resize(cube_side_count * cube_side_count);
		gl_uvs.resize(cube_side_count * cube_side_count);
		gl_normals.resize(cube_side_count * cube_side_count);

		for (side_id = 0; side_id < cube_side_count; ++side_id)
		{
			for (std::size_t i = 0; i < cube_side_count; ++i)
			{
				gl_vertices[side_id * cube_side_count + i] = raw::cubeVertices[side_id * cube_side_count + i];
				gl_uvs[side_id * cube_side_count + i] = raw::cubeTexCoords[i];
				gl_normals[side_id * cube_side_count + i] = raw::cubeNormals[side_id];
			}
		}

		mesh.setVertices(gl_vertices);
		mesh.setUVs(gl_uvs);
		mesh.setNormals(gl_normals);


		std::vector<glm::vec3> gl_tangents;
		std::vector<glm::vec3> gl_bitangents;

		tangents::computeTangentBasis(gl_vertices, gl_uvs, gl_normals, gl_tangents, gl_bitangents);

		mesh.setTangents(gl_tangents);
		mesh.setBitangents(gl_bitangents);


		// INDEX PART //
		/*std::vector<GLuint> gl_indices;
		static constexpr std::size_t indices_count = raw::vertices_per_triangle * raw::triangles_per_side;
		gl_indices.resize(indices_count);

		for (std::size_t i = 0; i < indices_count; ++i)
			gl_indices[i] = raw::indices[(side_id * raw::triangles_per_side) + (i / raw::vertices_per_triangle)][i % raw::vertices_per_triangle];

		mesh.setElements(gl_indices);*/

		return true;
	}
	
	const std::shared_ptr<ObjModel>& getModel()
	{
		if (CubeModel == nullptr)
		{
			CubeModel = std::make_shared<ObjModel>();

			for (unsigned int i = 0; i < 1; ++i)
			{
				if (!create_mesh(*CubeModel.get(), cube_side_id[i]))
					logger::fatal("CANNOT LOAD '{}' CUBE SIDE!!", cube_side_name[i]);
			}
		}

		return CubeModel;
	}
}
