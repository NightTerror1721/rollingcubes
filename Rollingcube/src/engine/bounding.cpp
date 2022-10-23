#include "bounding.h"

#include <limits>

#include "core/render.h"
#include "camera.h"


bool BoundingSphere::isOnFrustum(const Frustum& frustum, const Transformable& transf) const
{
	const glm::vec3 globalScale = transf.getGlobalScale();
	const glm::vec3 globalCenter = glm::vec3(transf.getModelMatrix() * glm::vec4(center, 1));
	const float maxScale = glm::max(glm::max(globalScale.x, globalScale.y), globalScale.z);

	BoundingSphere globalSphere = { globalCenter, radius * (maxScale * 0.5f) };

	return globalSphere.isOnFrustum(frustum);
}

void BoundingSphere::extract(const Model& model)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (const auto& mesh : model)
	{
		for (const auto& vertex : mesh->getVertices())
		{
			minAABB.x = glm::min(minAABB.x, vertex.x);
			minAABB.y = glm::min(minAABB.y, vertex.y);
			minAABB.z = glm::min(minAABB.z, vertex.z);

			maxAABB.x = glm::max(maxAABB.x, vertex.x);
			maxAABB.y = glm::max(maxAABB.y, vertex.y);
			maxAABB.z = glm::max(maxAABB.z, vertex.z);
		}
	}

	center = (maxAABB + minAABB) * 0.5f;
	radius = glm::length(minAABB - maxAABB);
}



bool SquareAABB::isOnFrustum(const Frustum& frustum, const Transformable& transf) const
{
	const glm::vec3 globalCenter = glm::vec3(transf.getModelMatrix() * glm::vec4(center, 1));
	
	const glm::vec3 right = transf.getRight() * extent;
	const glm::vec3 up = transf.getUp() * extent;
	const glm::vec3 forward = transf.getForward() * extent;

	const float newIi = glm::abs(glm::dot(glm::vec3{ 1, 0, 0 }, right)) +
		glm::abs(glm::dot(glm::vec3{ 1, 0, 0 }, up)) +
		glm::abs(glm::dot(glm::vec3{ 1, 0, 0 }, forward));

	const float newIj = glm::abs(glm::dot(glm::vec3{ 0, 1, 0 }, right)) +
		glm::abs(glm::dot(glm::vec3{ 0, 1, 0 }, up)) +
		glm::abs(glm::dot(glm::vec3{ 0, 1, 0 }, forward));

	const float newIk = glm::abs(glm::dot(glm::vec3{ 0, 0, 1 }, right)) +
		glm::abs(glm::dot(glm::vec3{ 0, 0, 1 }, up)) +
		glm::abs(glm::dot(glm::vec3{ 0, 0, 1 }, forward));

	const SquareAABB globalAABB = { globalCenter, glm::max(glm::max(newIi, newIj), newIk) };

	return globalAABB.isOnFrustum(frustum);
}

void SquareAABB::extract(const Model& model)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (const auto& mesh : model)
	{
		for (const auto& vertex : mesh->getVertices())
		{
			minAABB.x = glm::min(minAABB.x, vertex.x);
			minAABB.y = glm::min(minAABB.y, vertex.y);
			minAABB.z = glm::min(minAABB.z, vertex.z);

			maxAABB.x = glm::max(maxAABB.x, vertex.x);
			maxAABB.y = glm::max(maxAABB.y, vertex.y);
			maxAABB.z = glm::max(maxAABB.z, vertex.z);
		}
	}

	center = minAABB + (maxAABB - minAABB) / 2;
	extent = glm::max(glm::max(maxAABB.x, maxAABB.y), maxAABB.z);
}



bool AABB::isOnFrustum(const Frustum& frustum, const Transformable& transf) const
{
	const glm::vec3 globalCenter = glm::vec3(transf.getModelMatrix() * glm::vec4(center, 1));

	const glm::vec3 right = transf.getRight() * extents.x;
	const glm::vec3 up = transf.getUp() * extents.y;
	const glm::vec3 forward = transf.getForward() * extents.z;

	const float newIi = glm::abs(glm::dot(glm::vec3{ 1, 0, 0 }, right)) +
		glm::abs(glm::dot(glm::vec3{ 1, 0, 0 }, up)) +
		glm::abs(glm::dot(glm::vec3{ 1, 0, 0 }, forward));

	const float newIj = glm::abs(glm::dot(glm::vec3{ 0, 1, 0 }, right)) +
		glm::abs(glm::dot(glm::vec3{ 0, 1, 0 }, up)) +
		glm::abs(glm::dot(glm::vec3{ 0, 1, 0 }, forward));

	const float newIk = glm::abs(glm::dot(glm::vec3{ 0, 0, 1 }, right)) +
		glm::abs(glm::dot(glm::vec3{ 0, 0, 1 }, up)) +
		glm::abs(glm::dot(glm::vec3{ 0, 0, 1 }, forward));

	const AABB globalAABB = { globalCenter, newIi, newIj, newIk };

	return globalAABB.isOnFrustum(frustum);
}

void AABB::extract(const Model& model)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (const auto& mesh : model)
	{
		for (const auto& vertex : mesh->getVertices())
		{
			minAABB.x = glm::min(minAABB.x, vertex.x);
			minAABB.y = glm::min(minAABB.y, vertex.y);
			minAABB.z = glm::min(minAABB.z, vertex.z);

			maxAABB.x = glm::max(maxAABB.x, vertex.x);
			maxAABB.y = glm::max(maxAABB.y, vertex.y);
			maxAABB.z = glm::max(maxAABB.z, vertex.z);
		}
	}

	center = minAABB + (maxAABB - minAABB) / 2;
	extents = maxAABB;
}

void AABB::render(const Camera& cam, const Transformable& transf) const
{
	if (_linesShader == nullptr)
		_linesShader = ShaderProgramManager::instance().getLinesShaderProgram();

	if (_linesShader != nullptr)
	{
		auto vertices = getLinesVertices();

		_vao.createAttribute(
			0,
			gl::VAO::Attribute::ComponentCount::Three,
			gl::DataType::Float,
			GL_FALSE,
			0,
			std::addressof(vertices[0]),
			24,//vertices.size(),
			gl::VertexBufferUsage::DynamicDraw
		);

		_linesShader->use();
		_linesShader["model"] = transf.getModelMatrix();
		_linesShader["viewProjection"] = cam.getViewprojectionMatrix();
		_linesShader["color"] = glm::vec3(0, 1, 0);

		glPointSize(10);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(3);
		gl::render(_vao, GL_LINES);
		glPointSize(1);
		glLineWidth(1);
		glDisable(GL_LINE_SMOOTH);
		_linesShader->notUse();
	}
}
