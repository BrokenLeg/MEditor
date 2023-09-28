#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ScreenParams.h"

struct Camera
{
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;

	glm::vec3 position;
	float fov;
};

glm::mat4 getProjectionMatrix(const Camera& camera)
{
	float aspectRation = (float)DRAW_SECTION_WIDTH / (float)SCREEN_HEIGHT;
	return glm::perspective(camera.fov, aspectRation, 0.1f, 100.0f);
}

glm::mat4 getViewMatrix(const Camera& camera)
{
	return glm::lookAt(camera.position, camera.position + camera.front, camera.up);
}