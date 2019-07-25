#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
	glm::vec3 pos;
	glm::vec2 texCoord;
};

class Geometry
{
public:
	Geometry();
	~Geometry();

	static const Geometry geometry;

	const std::vector<Vertex> vertices = {
		{ { -1.0f, -1.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
		{ { -1.0f, 1.0f, 0.0f },{ 0.0f, 1.0f } }
	};

	// Careful, backface culling...
	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};
};

