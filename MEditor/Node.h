#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include "Mesh.h"

enum resourceType
{
	NONE = 0, //empty node only for grouping
	MESH, 
	LIGHT
};

struct Node
{
	char name[50];
	unsigned int resourceIndex;
	resourceType type;
	Transform transform;
	std::vector<Node*> children;
};