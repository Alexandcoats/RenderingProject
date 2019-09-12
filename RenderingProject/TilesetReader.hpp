#pragma once
#include "Material.hpp"
#include <unordered_map>
#include <glm/glm.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <vector>
#include <memory>
#include "json.hpp"

void readOBJ(std::string filepath, std::string matBaseDir, std::string metadataPath, std::vector<Tile> & tiles) {

	std::string extension = filepath.substr(filepath.size() - 4, 4);

	if (extension != ".obj") {
		throw std::runtime_error("Cannot load file: invalid file path " + filepath);
	}

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str(), matBaseDir.c_str()))
		throw std::runtime_error(warn + err);

	if (materials.empty()) throw std::runtime_error("No materials found for object.");

	std::sort(shapes.begin(), shapes.end(), [](tinyobj::shape_t a, tinyobj::shape_t b) { return a.name < b.name;  });

	using json = nlohmann::json;

	// read a JSON file
	std::ifstream metadata(metadataPath);
	json j;
	metadata >> j;

	std::unordered_map<std::string, std::unordered_map<int, std::shared_ptr<Material>>> tileMeshes;
	std::unordered_map<std::string, std::vector<std::shared_ptr<Material>>> subMeshes;

	for (const auto & shape : shapes) {

		std::unordered_map<Vertex, unsigned int> uniqueVertices = {};

		std::vector<float> location = j[shape.name]["location"];

		for (int index_offset = 0, face = 0; face < shape.mesh.num_face_vertices.size(); ++face) {
			int face_verts = shape.mesh.num_face_vertices[face];

			for (int face_vert = 0; face_vert < face_verts; ++face_vert) {
				const auto & index = shape.mesh.indices[index_offset + face_vert];
				Vertex vertex = {};
				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0] + location[0],
					attrib.vertices[3 * index.vertex_index + 1] + location[2],
					attrib.vertices[3 * index.vertex_index + 2] + location[1]
				};

				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				int mat_index = shape.mesh.material_ids[face];

				if (mat_index >= materials.size()) throw std::runtime_error("A material does not exist for this face!");

				if (tileMeshes.count(shape.name) == 0 || tileMeshes[shape.name].count(mat_index) == 0) tileMeshes[shape.name][mat_index] = std::make_shared<Material>(materials[mat_index]);

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<unsigned int>(tileMeshes[shape.name][mat_index]->vertices.size());
					tileMeshes[shape.name][mat_index]->vertices.push_back(vertex);
				}
				tileMeshes[shape.name][mat_index]->indices.push_back(uniqueVertices[vertex]);
			}
			index_offset += face_verts;
		}
		std::vector<std::shared_ptr<Material>> mats;
		for (auto e : tileMeshes[shape.name]) {
			e.second->createBuffers();
			mats.push_back(e.second);
		}

		if (shape.name.substr(0, 4) == "tile") {
			tiles.push_back(Tile{ mats, glm::vec3(location[0], location[2], location[1]) });
		} else {
			subMeshes[shape.name] = mats;
		}
	}

	for (auto& e : j.items()) {
		if (e.key().substr(0, 4) == "tile" && e.key().substr(5, 1) == "_") {
			// We can get the reference shape from the name, but it may be better to include this in the metadata
			std::string refShape = e.key().substr(6,e.key().length() - 7);
			if (subMeshes.count(refShape) > 0) {
				std::vector<float> location = e.value()["location"];
				std::vector<float> refRotation = j[refShape]["rotation"];
				std::vector<float> rotation = e.value()["rotation"];
				int index = std::stoi(e.key().substr(4, 1));
				tiles[index].subMeshes.push_back(SubMesh{
					subMeshes[refShape],
					glm::vec3(location[0] - tiles[index].location.x, location[2] - tiles[index].location.y, location[1] - tiles[index].location.z),
					glm::vec3(rotation[0], rotation[2], rotation[1])
					});
			}
		}
	}
}