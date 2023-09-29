#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

unsigned int POINT_VAO;
const glm::vec3 WHITE_COLOR = { 1.0f, 1.0f, 1.0f };

const unsigned int MAX_NAME_LENGTH = 50;

struct Mesh
{
	glm::vec3 position; //also represents mesh center
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::vec3 fillColor; //move to materials
	glm::vec3 edgesColor;

	char name[MAX_NAME_LENGTH];
	unsigned int fillVAO;
	unsigned int edgesVAO;
	unsigned int fillIndicesCount;
	unsigned int edgesIndicesCount;
};

glm::mat4 getModelMatrix(const Mesh& mesh)
{
	glm::mat4 translate(1.0f);
	glm::mat4 rotateX(1.0f), rotateY(1.0f), rotateZ(1.0f), rotate(1.0f);
	glm::mat4 scale(1.0f);

	translate = glm::translate(translate, mesh.position);
	rotateX = glm::rotate(rotateX, mesh.rotation.x, { 1, 0, 0 });
	rotateY = glm::rotate(rotateY, mesh.rotation.y, { 0, 1, 0 });
	rotateZ = glm::rotate(rotateZ, mesh.rotation.z, { 0, 0, 1 });

	rotate = rotateZ * rotateY * rotateX;

	scale = glm::scale(scale, mesh.scale);

	return translate * rotate * scale;
}

void drawMesh(const Mesh& mesh, Shader& shader, bool edges, bool center)
{
	glBindVertexArray(mesh.fillVAO);
	shader.SetVector3f("color", mesh.fillColor);
	shader.SetMatrix4("model", getModelMatrix(mesh));
	glDrawElements(GL_TRIANGLES, mesh.fillIndicesCount, GL_UNSIGNED_INT, 0);

	if (edges)
	{
		glBindVertexArray(mesh.edgesVAO);
		shader.SetVector3f("color", mesh.edgesColor);
		glLineWidth(1.0f);
		glDrawElements(GL_LINES, mesh.edgesIndicesCount, GL_UNSIGNED_INT, 0);
	}

	if (center)
	{
		glBindVertexArray(POINT_VAO);
		glDisable(GL_DEPTH_TEST);
		shader.SetVector3f("color", WHITE_COLOR);
		glPointSize(5.0f);
		glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
	}
}
