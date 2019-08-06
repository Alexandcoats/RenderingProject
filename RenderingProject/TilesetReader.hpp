#pragma once
#include "MeshObject.hpp"
#include <unordered_map>
#include <glm/glm.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <vector>

std::vector<std::unique_ptr<MeshObject>> readOBJ(std::string filepath, int vertexLocation, int normalLocation, int uvLocation) {

	std::string extension = filepath.substr(filepath.size() - 4, 4);

	if (extension != ".obj") {
		throw std::runtime_error("Cannot load file: invalid file path " + filepath);
	}

	int locations[3] = { vertexLocation, normalLocation, uvLocation };

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		throw std::runtime_error(warn + err);

	std::vector<std::unique_ptr<MeshObject>> meshes;

	for (const auto & shape : shapes) {

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		std::unordered_map<Vertex, unsigned int> uniqueVertices = {};

		for (const auto & index : shape.mesh.indices) {
			Vertex vertex = {};
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (locations[1] != -1) {
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}

			if (locations[2] != -1) {
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<unsigned int>(vertices.size());
				vertices.push_back(vertex);
			}
			indices.push_back(uniqueVertices[vertex]);
		}

		meshes.push_back(std::move(std::make_unique<MeshObject>(locations, vertices, indices)));
	}

	return meshes;
}