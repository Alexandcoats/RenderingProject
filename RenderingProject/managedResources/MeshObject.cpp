#define TINYOBJLOADER_IMPLEMENTATION
#include "MeshObject.h"

void MeshObject::createResource(
	std::vector<GLfloat> vertexData, 
	std::vector<GLfloat> normalData, 
	std::vector<GLfloat> uvData, 
	std::vector<GLuint>  indexData)
{
	//std::cout << "Creating model VBOs in OpenGL" << std::endl;

	//create vertex buffer
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertexData.size() * sizeof(GLfloat),
		&vertexData.front(),
		GL_STATIC_DRAW
	);

	if (!normalData.empty())
	{
		//create normal buffer
		glGenBuffers(1, &normalID);
		glBindBuffer(GL_ARRAY_BUFFER, normalID);
		glBufferData(
			GL_ARRAY_BUFFER,
			normalData.size() * sizeof(GLfloat),
			&normalData.front(),
			GL_STATIC_DRAW
		);
	}

	if (!uvData.empty())
	{
		//create UV buffer
		glGenBuffers(1, &uvID);
		glBindBuffer(GL_ARRAY_BUFFER, uvID);
		glBufferData(
			GL_ARRAY_BUFFER,
			uvData.size() * sizeof(GLfloat),
			&uvData.front(),
			GL_STATIC_DRAW
		);
	}

	//create index buffer
	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indexData.size() * sizeof(GLuint),
		&indexData.front(),
		GL_STATIC_DRAW
	);

	//store size of index buffer
	indexSize = indexData.size();
}

void MeshObject::bindResource()
{
	//enable use of vertex attribute
	glEnableVertexAttribArray(0);
	//set focus to this object's vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	//point attribute to buffer
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	if (normalID != -1)
	{
		//enable use of normal attribute
		glEnableVertexAttribArray(1);
		//set focus to this object's normal buffer
		glBindBuffer(GL_ARRAY_BUFFER, normalID);
		//point attribute to buffer
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
	}

	if (uvID != -1)
	{
		//enable use of UV attribute
		glEnableVertexAttribArray(2);
		//set focus to this object's UV buffer
		glBindBuffer(GL_ARRAY_BUFFER, uvID);
		//point attribute to buffer
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
	}
}

void MeshObject::freeResource()
{
	glDisableVertexAttribArray(0);
	if (normalID != -1)
		glDisableVertexAttribArray(1);
	if (uvID != -1)
		glDisableVertexAttribArray(2);
}

void MeshObject::deleteResource()
{
	GLuint buffers[4] = { vertexID, normalID, uvID, indexID };
	glDeleteBuffers(4, buffers);
}

MeshObject::MeshObject(std::string filepath) : ObjectResource(filepath)
{
	std::string extension = filepath.substr(filepath.size() - 5, 4);
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

MeshObject::~MeshObject()
{
	deleteResource();
}

GLuint MeshObject::getVertexID()
{
	return vertexID;
}

GLuint MeshObject::getNormalID()
{
	return normalID;
}

GLuint MeshObject::getUVID()
{
	return uvID;
}

GLuint MeshObject::getIndexID()
{
	return indexID;
}

GLsizei MeshObject::getIndexSize()
{
	return indexSize;
}

void MeshObject::readOBJ(std::string filepath)
{
	//std::cout <<"Reading OBJ file: " << filepath << std::endl;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	//find working directory
	std::string fullPath = getWorkingDir() + filepath;

	bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fullPath.c_str());

	if (!err.empty() || !success)
	{
		throw std::runtime_error("Error loading file " + filepath);
	}

	//convert to indexed data
	std::unordered_map<std::string, int> face_pts;

	std::vector<GLuint> indexes;
	std::vector<GLfloat> vertexes_indexed;
	std::vector<GLfloat> normals_indexed;
	std::vector<GLfloat> uvs_indexed;

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
	this->createResource(vertexes_indexed, normals_indexed, uvs_indexed, indexes);
}

void MeshObject::readPXO(std::string filepath)
{
	//std::cout << "Reading PXO file: " << filepath << std::endl;

	//find working directory
	std::string fullPath = getWorkingDir() + filepath;

	std::ifstream inFile(fullPath, std::ios::binary);

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
	std::vector<uint32_t> indexes(indexSize / sizeof(uint32_t));

	inFile.read(reinterpret_cast<char*>(&verts.front()), vertSize);
	if (normSize > 0) inFile.read(reinterpret_cast<char*>(&norms.front()), normSize);
	if (uvSize > 0) inFile.read(reinterpret_cast<char*>(&uvs.front()), uvSize);
	inFile.read(reinterpret_cast<char*>(&indexes.front()), indexSize);

	this->createResource(verts, norms, uvs, indexes);
}
