#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "ObjectResource.h"
#include "MeshObject.h"
#include "TextureObject.h"
#include "ShaderObject.h"
#include "fontObject.h"

#include <unordered_map>
#include <string>
#include <memory>

/*
  Manages data stored in the OpenGl context. 
  Keeps a map of loaded resources keyed to an identifier, typically a filepath.
  When adding a resource, this manager will check if the resource is already registered
  and return a shared pointer to either the newly created or already existing resource.
  Also inculdes a flush function to clear any resources that no longer are referenced to.

  Each resource will keep record of the loaded data's OpenGL ID and has basic 
  functions for binding, freeing, and deleting the resource through OpenGL.

  Currently supports:
  mesh/geometry data
  textures
  shaders
  fonts

  See each resource type's header file for further functionality
*/

class ResourceManager
{
private:
	std::unordered_map < std::string, std::weak_ptr<ObjectResource> > observers;

public:
	ResourceManager();
	~ResourceManager();

	std::unordered_map < std::string, std::weak_ptr<ObjectResource> > getMap();
		
	std::shared_ptr<MeshObject> addMesh(std::string);
	std::shared_ptr<TextureObject> addTexture(std::string);
	std::shared_ptr<ShaderObject> addShader(std::string);
	std::shared_ptr<FontObject> addFont(std::string);

	void flushExpired();
};

#endif //RESOURCE_MANAGER_H