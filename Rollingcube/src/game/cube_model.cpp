#include "cube_model.h"

#include "utils/logger.h"


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


		static constexpr glm::vec3 normals[cube_side_count] = {
			{ 0, 1, 0 },  // up //
			{ 0, -1, 0 }, // down //
			{ 1, 0, 0 },  // left //
			{ -1, 0, 0 }, // right //
			{ 0, 0, 1 },  // front //
			{ 0, 0, -1 }  // back //
		};

		static constexpr glm::vec2 tex_coords[vertices_per_side] = {
			{ 0, 0 }, // down-left //
			{ 1, 0 }, // down-right //
			{ 0, 1 }, // up-left //
			{ 1, 1 }, // up-right //
		};

		static constexpr glm::vec3 vertices[vertices_count] = {
			{ 1, 1, -1 },	// up-face: down-left //
			{ -1, 1, -1 },	// up-face: down-right //
			{ 1, 1, 1 },	// up-face: up-left //
			{ -1, 1, 1 },	// up-face: up-right //

			{ 1, -1, -1 },	// down-face: down-left //
			{ -1, -1, -1 },	// down-face: down-right //
			{ 1, -1, 1 },	// down-face: up-left //
			{ -1, -1, 1 }	// down-face: up-right //
		};

		static constexpr CubeVertex cube_vertices[cube_side_count * cube_side_count] = {
			// up //
			{ vertices[0], normals[0], tex_coords[0] },
			{ vertices[1], normals[0], tex_coords[1] },
			{ vertices[2], normals[0], tex_coords[2] },
			{ vertices[1], normals[0], tex_coords[1] },
			{ vertices[2], normals[0], tex_coords[2] },
			{ vertices[3], normals[0], tex_coords[3] },

			// down //
			{ vertices[4], normals[1], tex_coords[2] },
			{ vertices[5], normals[1], tex_coords[3] },
			{ vertices[6], normals[1], tex_coords[0] },
			{ vertices[5], normals[1], tex_coords[3] },
			{ vertices[6], normals[1], tex_coords[0] },
			{ vertices[7], normals[1], tex_coords[1] },

			// left //
			{ vertices[6], normals[2], tex_coords[0] },
			{ vertices[4], normals[2], tex_coords[1] },
			{ vertices[2], normals[2], tex_coords[2] },
			{ vertices[4], normals[2], tex_coords[1] },
			{ vertices[2], normals[2], tex_coords[2] },
			{ vertices[0], normals[2], tex_coords[3] },

			// right //
			{ vertices[5], normals[3], tex_coords[2] },
			{ vertices[7], normals[3], tex_coords[3] },
			{ vertices[1], normals[3], tex_coords[0] },
			{ vertices[7], normals[3], tex_coords[3] },
			{ vertices[1], normals[3], tex_coords[0] },
			{ vertices[3], normals[3], tex_coords[1] },

			// front //
			{ vertices[6], normals[4], tex_coords[2] },
			{ vertices[7], normals[4], tex_coords[3] },
			{ vertices[2], normals[4], tex_coords[0] },
			{ vertices[7], normals[4], tex_coords[3] },
			{ vertices[2], normals[4], tex_coords[0] },
			{ vertices[3], normals[4], tex_coords[1] },

			// back //
			{ vertices[4], normals[5], tex_coords[0] },
			{ vertices[5], normals[5], tex_coords[1] },
			{ vertices[0], normals[5], tex_coords[2] },
			{ vertices[5], normals[5], tex_coords[1] },
			{ vertices[0], normals[5], tex_coords[2] },
			{ vertices[1], normals[5], tex_coords[3] }
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
		gl_vertices.resize(cube_side_count);
		gl_uvs.resize(cube_side_count);
		gl_normals.resize(cube_side_count);

		for (std::size_t i = 0; i < cube_side_count; ++i)
		{
			const auto& vertex = raw::cube_vertices[side_id * cube_side_count];
			gl_vertices[i] = vertex.position;
			gl_uvs[i] = vertex.tex_coord;
			gl_normals[i] = vertex.normal;
		}

		mesh.setVertices(gl_vertices);
		mesh.setUVs(gl_uvs);
		mesh.setNormals(gl_normals);


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

			for (unsigned int i = 0; i < cube_side_count; ++i)
			{
				if (!create_mesh(*CubeModel.get(), cube_side_id[i]))
					logger::fatal("CANNOT LOAD '{}' CUBE SIDE!!", cube_side_name[i]);
			}
		}

		return CubeModel;
	}
}
