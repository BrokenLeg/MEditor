#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

unsigned int POINT_VAO;
const glm::vec3 WHITE_COLOR = { 1.0f, 1.0f, 1.0f };

const unsigned int MAX_NAME_LENGTH = 50;

struct Orientation
{
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	float yaw;
	float pitch;
};

struct Transform
{
	glm::vec3 position; //also represents mesh center
	glm::vec3 scale;
	glm::vec3 orientation;
};

struct Mesh
{
	Transform trf;

	glm::vec3 fillColor; //move to materials
	glm::vec3 edgesColor;

	char name[MAX_NAME_LENGTH];

	unsigned int fillVAO;
	unsigned int edgesVAO;
	unsigned int fillIndicesCount;
	unsigned int edgesIndicesCount;
};

glm::vec3 getEulerFromMatrix(const glm::mat4& R)
{
	float x = glm::degrees(atan2(R[1][2], R[2][2]));
	float y = -glm::degrees(asin(R[0][2]));
	float z = glm::degrees(atan2(R[0][1], R[0][0]));
	return {x, y, z};
}

glm::mat4 getRotationMatrixZYX(const glm::vec3& eulerZYX)
{
	glm::vec3 eX = { 1, 0, 0 };
	glm::vec3 eY = { 0, 1, 0 };
	glm::vec3 eZ = { 0, 0, 1 };

	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(eulerZYX.x), eX);
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(eulerZYX.y), eY);
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(eulerZYX.z), eZ);

	return rotateZ * rotateY * rotateX;
}

void applyGlobalRotation(Transform& trf, const glm::vec3 axis, float angle)
{
	glm::mat4 currentRotation = getRotationMatrixZYX(trf.orientation);
	glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
	glm::mat4 finalRotationMatrix = rotationX * currentRotation;
	trf.orientation = getEulerFromMatrix(finalRotationMatrix);
}

glm::mat4 getModelMatrix(const Transform& trf)
{
	glm::mat4 translate(1.0f);
	glm::mat4 scale(1.0f);

	translate = glm::translate(translate, trf.position);

	scale = glm::scale(scale, trf.scale);

	return translate * getRotationMatrixZYX(trf.orientation) * scale;
}

void drawMesh(const Mesh& mesh, Shader& shader, const glm::mat4& trf, bool edges, bool center)
{
	glBindVertexArray(mesh.fillVAO);
	shader.SetVector3f("color", mesh.fillColor);
	shader.SetMatrix4("model", trf);
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
