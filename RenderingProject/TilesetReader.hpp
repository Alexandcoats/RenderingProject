#pragma once
#include "MeshObject.hpp"
#include <unordered_map>
#include <glm/glm.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <vector>
#include <memory>
#include "json.hpp"

void readOBJ(std::string filepath, std::string metadataPath, int vertexLocation, int normalLocation, int uvLocation, std::vector<Tile> & tiles) {

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

	std::sort(shapes.begin(), shapes.end(), [](tinyobj::shape_t a, tinyobj::shape_t b) { return a.name < b.name;  });

	using json = nlohmann::json;

	// read a JSON file
	std::ifstream metadata(metadataPath);
	json j;
	metadata >> j;

	std::unordered_map<std::string, std::shared_ptr<MeshObject>> meshes;

	for (const auto & shape : shapes) {

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		std::unordered_map<Vertex, unsigned int> uniqueVertices = {};

		std::vector<float> location = j[shape.name]["location"];

		for (const auto & index : shape.mesh.indices) {
			Vertex vertex = {};
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0] + location[0],
				attrib.vertices[3 * index.vertex_index + 1] + location[2],
				attrib.vertices[3 * index.vertex_index + 2] + location[1]
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

		if (shape.name.substr(0, 4) == "tile") {
			tiles.push_back(Tile{ std::make_unique<MeshObject>(locations, vertices, indices), glm::vec3(location[0], location[2], location[1]) });
		}
		else 
			meshes[shape.name] = std::move(std::make_shared<MeshObject>(locations, vertices, indices));
	}

	for (auto& e : j.items()) {
		if (e.key().substr(0, 4) == "tile" && e.key().substr(5, 1) == "_") {
			// We can get the reference shape from the name, but it may be better to include this in the metadata
			std::string refShape = e.key().substr(6,e.key().length() - 7);
			std::vector<float> location = e.value()["location"];
			int index = std::stoi(e.key().substr(4, 1));
			tiles[index].subMeshes.push_back(SubMesh{ meshes[refShape], glm::vec3(location[0] - tiles[index].location.x, location[2] - tiles[index].location.y, location[1] - tiles[index].location.z) });
		}
	}
}