#include "ObjectResource.h"

void ObjectResource::bindResource()
{
	throw std::runtime_error("Using base class for resource bind. please use an inherited class.");
}

void ObjectResource::freeResource()
{
	throw std::runtime_error("Using base class for resource freeing. please use an inherited class.");
}

void ObjectResource::deleteResource()
{
	throw std::runtime_error("Using base class for resource deletion. please use an inherited class.");
}

ObjectResource::ObjectResource(std::string filepath)
{
	this->filepath = filepath;
}

ObjectResource::~ObjectResource()
{
	deleteResource();
}

std::string ObjectResource::getFilepath()
{
	return filepath;
}
