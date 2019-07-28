#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

std::unordered_map<std::string, std::weak_ptr<ObjectResource>> ResourceManager::getMap()
{
	return observers;
}


std::shared_ptr<MeshObject> ResourceManager::addMesh(std::string filepath)
{
	auto it = observers.find(filepath);
	if (it != observers.end())
	{
		std::shared_ptr<MeshObject> found = std::dynamic_pointer_cast<MeshObject> ((*it).second.lock());
		if (found)
			return found;
	}

	std::shared_ptr<MeshObject> newResource = std::make_shared<MeshObject>(filepath);
	observers[filepath] = newResource;
	return newResource;
}

std::shared_ptr<TextureObject> ResourceManager::addTexture(std::string filepath)
{
	auto it = observers.find(filepath);
	if (it != observers.end())
	{
		std::shared_ptr<TextureObject> found = std::dynamic_pointer_cast<TextureObject> ((*it).second.lock());
		if (found)
			return found;
	}

	std::shared_ptr<TextureObject> newResource = std::make_shared<TextureObject>(filepath);
	observers[filepath] = newResource;
	return newResource;
}

std::shared_ptr<ShaderObject> ResourceManager::addShader(std::string filepath)
{
	auto it = observers.find(filepath);
	if (it != observers.end())
	{
		std::shared_ptr<ShaderObject> found = std::dynamic_pointer_cast<ShaderObject> ((*it).second.lock());
		if (found)
			return found;
	}

	std::shared_ptr<ShaderObject> newResource = std::make_shared<ShaderObject>(filepath);
	observers[filepath] = newResource;
	return newResource;
}

std::shared_ptr<FontObject> ResourceManager::addFont(std::string filepath)
{
	auto it = observers.find(filepath);
	if (it != observers.end())
	{
		std::shared_ptr<FontObject> found = std::dynamic_pointer_cast<FontObject> ((*it).second.lock());
		if (found)
			return found;
	}

	std::shared_ptr<FontObject> newResource = std::make_shared<FontObject>(filepath);
	observers[filepath] = newResource;
	return newResource;
}

void ResourceManager::flushExpired()
{
	for (auto it = observers.begin(); it != observers.end(); )
	{
		if ((*it).second.expired())
			observers.erase(it++);
		else
			it++;
	}
}
