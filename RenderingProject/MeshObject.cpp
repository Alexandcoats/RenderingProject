#define TINYOBJLOADER_IMPLEMENTATION
#include "MeshObject.hpp"

MeshObject::MeshObject(std::string filepath, unsigned int vertexLocation, unsigned int normalLocation, unsigned int uvLocation) : filepath(filepath) {
	locations[0] = vertexLocation;
	locations[1] = normalLocation;
	locations[2] = uvLocation;
	std::string extension = filepath.substr(filepath.size() - 4, 4);
	//read file
	if (extension == ".obj")
	{
		readOBJ(filepath);
	}
	else if (extension == ".pxo")
	{
		readPXO(filepath);
	}
	else
	{
		throw std::runtime_error("Cannot load file: invalid file path " + filepath);
	}
}

void MeshObject::draw() {
	vao->bind();
	glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, (void*)0);
	vao->unbind();
}

void MeshObject::createBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data) {
	glGenBuffers(1, bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, *bufferID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void MeshObject::createIndexBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data) {
	glGenBuffers(1, bufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}


void MeshObject::createBuffers(std::vector<float> vertexData, std::vector<float> normalData, std::vector<float> uvData, std::vector<unsigned int> indexData) {
	//std::cout << "Creating model VBOs in OpenGL" << std::endl;

	vao = new VertexArrayObject();
	vao->bind();

	//create vertex buffer
	createBuffer(&bufferIDs[0], vertexData.size() * sizeof(float), &vertexData.front());

	//create index buffer
	createIndexBuffer(&bufferIDs[3], indexData.size() * sizeof(unsigned int), &indexData.front());

	vao->writeVertexAttribute(locations[0], 3, 0, (void*)0);

	if (locations[1] && !normalData.empty())
	{
		//create normal buffer
		createBuffer(&bufferIDs[1], normalData.size() * sizeof(float), &normalData.front());
		vao->writeVertexAttribute(locations[1], 3, 0, (void*)0);
	}

	if (locations[2] && !uvData.empty())
	{
		//create UV buffer
		createBuffer(&bufferIDs[2], uvData.size() * sizeof(float), &uvData.front());
		vao->writeVertexAttribute(locations[2], 2, 0, (void*)0);
	}

	vao->unbind();

	indexSize = indexData.size();
}

void MeshObject::readOBJ(std::string filepath) {
	//std::cout <<"Reading OBJ file: " << filepath << std::endl;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str());

	if (!err.empty() || !success)
	{
		throw std::runtime_error("Error loading file " + filepath);
	}

	//convert to indexed data
	std::unordered_map<std::string, int> face_pts;

	std::vector<unsigned int> indexes;
	std::vector<float> vertexes_indexed;
	std::vector<float> normals_indexed;
	std::vector<float> uvs_indexed;

	//reserve worst-case max size
	face_pts.reserve(shapes[0].mesh.indices.size());
	indexes.reserve(shapes[0].mesh.indices.size());
	vertexes_indexed.reserve(attrib.vertices.size());
	normals_indexed.reserve(attrib.normals.size());
	uvs_indexed.reserve(attrib.texcoords.size());

	int indexedCount = 0;
	for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
	{
		int vert_idx = shapes[0].mesh.indices[i].vertex_index;
		int tex_idx = shapes[0].mesh.indices[i].texcoord_index;
		int norm_idx = shapes[0].mesh.indices[i].normal_index;

		//check if already indexed
		std::string findThis;
		findThis = std::to_string(vert_idx) + '/' + std::to_string(tex_idx) + '/' + std::to_string(norm_idx);

		if (face_pts.find(findThis) != face_pts.end())
		{
			//point is already indexed
			indexes.push_back(face_pts[findThis]);
		}
		else  //new point
		{
			//add vertex
			if (vert_idx != -1)
			{
				vertexes_indexed.push_back(attrib.vertices[3 * vert_idx + 0]);
				vertexes_indexed.push_back(attrib.vertices[3 * vert_idx + 1]);
				vertexes_indexed.push_back(attrib.vertices[3 * vert_idx + 2]);
			}

			//add UV
			if (tex_idx != -1)
			{
				uvs_indexed.push_back(attrib.texcoords[2 * tex_idx + 0]);
				uvs_indexed.push_back(attrib.texcoords[2 * tex_idx + 1]);
			}

			//add normal
			if (norm_idx != -1)
			{
				normals_indexed.push_back(attrib.normals[3 * norm_idx + 0]);
				normals_indexed.push_back(attrib.normals[3 * norm_idx + 1]);
				normals_indexed.push_back(attrib.normals[3 * norm_idx + 2]);
			}

			//add index
			indexes.push_back(indexedCount);

			//add to face records
			face_pts.insert({ findThis, indexedCount++ });
		}
	}

	//trim data footprint
	vertexes_indexed.shrink_to_fit();
	normals_indexed.shrink_to_fit();
	uvs_indexed.shrink_to_fit();
	indexes.shrink_to_fit();

	//pass data to OpenGL
	this->createBuffers(vertexes_indexed, normals_indexed, uvs_indexed, indexes);
}

void MeshObject::readPXO(std::string filepath) {
	//std::cout << "Reading PXO file: " << filepath << std::endl;

	std::ifstream inFile(filepath, std::ios::binary);

	//get container sizes
	size_t vertSize, normSize, uvSize, indexSize;
	inFile.read(reinterpret_cast<char*>(&vertSize), sizeof(size_t));
	inFile.read(reinterpret_cast<char*>(&normSize), sizeof(size_t));
	inFile.read(reinterpret_cast<char*>(&uvSize), sizeof(size_t));
	inFile.read(reinterpret_cast<char*>(&indexSize), sizeof(size_t));

	//fetch data
	std::vector<float> verts(vertSize / sizeof(float));
	std::vector<float> norms(normSize / sizeof(float));
	std::vector<float> uvs(uvSize / sizeof(float));
	std::vector<unsigned int> indexes(indexSize / sizeof(unsigned int));

	inFile.read(reinterpret_cast<char*>(&verts.front()), vertSize);
	if (normSize > 0) inFile.read(reinterpret_cast<char*>(&norms.front()), normSize);
	if (uvSize > 0) inFile.read(reinterpret_cast<char*>(&uvs.front()), uvSize);
	inFile.read(reinterpret_cast<char*>(&indexes.front()), indexSize);

	this->createBuffers(verts, norms, uvs, indexes);
}

MeshObject::~MeshObject() {
	glDeleteBuffers(4, bufferIDs);
}