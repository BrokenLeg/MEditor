#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE)
	{
		return;
	}

	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

	//panning
	if (key == GLFW_KEY_A)
	{
		camera->position -= camera->right * 0.1f;
	}

	if (key == GLFW_KEY_D)
	{
		camera->position += camera->right * 0.1f;
	}

	if (key == GLFW_KEY_W)
	{
		camera->position -= camera->up * 0.1f;
	}

	if (key == GLFW_KEY_S)
	{
		camera->position += camera->up * 0.1f;
	}
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

	if (yoffset < 0)
	{
		camera->position -= camera->front * 0.1f;
	}
	else
	{
		camera->position += camera->front * 0.1f;
	}
}