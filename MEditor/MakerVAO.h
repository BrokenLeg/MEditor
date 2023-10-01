#pragma once
#include <GL/glew.h>
#include "Mesh.h"

unsigned int createVAO(const float* vertices, unsigned int verticesCount,
	const unsigned int* indices, unsigned int indicesCount)
{
	unsigned int VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, verticesCount, vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	return VAO;
}

void createCubeVAO(Mesh& cube)
{
	float vertices[] =
	{
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
	};

	unsigned int fillIndices[] =
	{
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		3, 7, 6,
		6, 2, 3,

		0, 4, 5,
		5, 1, 0,

		0, 4, 7,
		7, 3, 0,

		2, 6, 5,
		5, 1, 2
	};

	unsigned int edgesIndices[] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,

		0, 4,
		4, 7,
		7, 3,
		3, 0,

		3, 7,
		7, 6,
		6, 2,
		2, 3,

		7, 4,
		4, 5,
		5, 6,
		6, 7,

		2, 6,
		6, 5,
		5, 1,
		1, 2,

		0, 4,
		4, 5,
		5, 1,
		1, 0
	};

	cube.fillVAO = createVAO(vertices, sizeof(vertices), fillIndices, sizeof(fillIndices));
	cube.fillIndicesCount = sizeof(fillIndices);

	cube.edgesVAO = createVAO(vertices, sizeof(vertices), edgesIndices, sizeof(edgesIndices));
	cube.edgesIndicesCount = sizeof(edgesIndices);
}

void createTriangleVAO(Mesh& triangle)
{
	float vertices[] =
	{
		-1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	unsigned int fillIndices[] =
	{
		0, 1, 2
	};

	unsigned int edgesIndices[] =
	{
		0, 1,
		1, 2,
		2, 0
	};

	triangle.fillVAO = createVAO(vertices, sizeof(vertices), fillIndices, sizeof(fillIndices));
	triangle.fillIndicesCount = sizeof(fillIndices);

	triangle.edgesVAO = createVAO(vertices, sizeof(vertices), edgesIndices, sizeof(edgesIndices));
	triangle.edgesIndicesCount = sizeof(edgesIndices);
}

unsigned int createPointVAO()
{
	float vertices[] =
	{
		0.0f, 0.0f, 0.0f
	};

	unsigned int indices[] =
	{
		0
	};

	return createVAO(vertices, sizeof(vertices), indices, sizeof(indices));
}
