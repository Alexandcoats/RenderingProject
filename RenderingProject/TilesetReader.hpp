#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "json.hpp"
#include "Tile.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

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

		glm::vec3 location = j.count(shape.name) > 0 ? glm::vec3(j[shape.name]["location"][0], j[shape.name]["location"][1], j[shape.name]["location"][2])  : glm::vec3(0.0f, 0.0f, 0.0f);

		for (int index_offset = 0, face = 0; face < shape.mesh.num_face_vertices.size(); ++face) {
			int face_verts = shape.mesh.num_face_vertices[face];

			for (int face_vert = 0; face_vert < face_verts; ++face_vert) {
				const auto & index = shape.mesh.indices[index_offset + face_vert];
				Vertex vertex = {};
				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0] + location.x,
					attrib.vertices[3 * index.vertex_index + 1] + location.z,
					attrib.vertices[3 * index.vertex_index + 2] + location.y
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
			tiles.push_back(Tile{ mats });
		} else {
			subMeshes[shape.name] = mats;
		}
	}

	for (auto & e : j.items()) {
		std::string refShape = e.key();
		int index = std::stoi(refShape.substr(4, 1));
		glm::vec3 refLoc = { j[refShape]["location"][0], j[refShape]["location"][2], j[refShape]["location"][1] };
		for (auto & e2 : j[refShape]["subMeshes"].items()) {
			std::string subMeshShape = e2.key();
			subMeshShape = subMeshShape.substr(0, subMeshShape.length() - 1);
			auto sm = e2.value();
			if (subMeshes.count(subMeshShape) > 0) {
				glm::vec3 location = { sm["location"][0], sm["location"][2], sm["location"][1] };
				glm::vec3 rotation = { sm["rotation"][0], sm["rotation"][2], sm["rotation"][1] };
				tiles[index].subMeshes.push_back(SubMesh{
					subMeshes[subMeshShape],
					location - refLoc,
					rotation
					});
			}
		}
		for (auto & l : e.value()["lights"]) {
			glm::vec3 pos = { l["pos"][0], l["pos"][2], l["pos"][1] };
			glm::vec3 color = { l["color"][0], l["color"][1], l["color"][2] };
			tiles[index].lights.push_back({ pos - refLoc, color, l["brightness"] });
		}
	}
}