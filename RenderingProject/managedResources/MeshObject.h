#ifndef MESH_OBJECT_H
#define MESH_OBJECT_H

#include <algorithm>
#include <unordered_map>
#include <glm\glm.hpp>
#include <tiny_OBJ_loader\tiny_obj_loader.h>

#include "ObjectResource.h"

/*
  Resource type for loading vertex geometry data.
  Data is stored into OpenGL in the form of mutliple attribute arryas

  Currently supports:
  vertex(location 0)
  normal(location 1)
  UV(location 2)
  index
*/

class MeshObject : public ObjectResource
{
private:
	GLuint vertexID = -1;
	GLuint normalID = -1;
	GLuint uvID = -1;
	GLuint indexID = -1;

	GLsizei indexSize;

public:
	void createResource(
		std::vector<GLfloat>, 
		std::vector<GLfloat>, 
		std::vector<GLfloat>, 
		std::vector<GLuint>);

	virtual void bindResource();
	virtual void freeResource();
	virtual void deleteResource();

	MeshObject(std::string);
	virtual ~MeshObject();

	GLuint getVertexID();
	GLuint getNormalID();
	GLuint getUVID();
	GLuint getIndexID();
	GLsizei getIndexSize();

	void readOBJ(std::string);
	void readPXO(std::string);
};

#endif //MESH_OBJECT_H
