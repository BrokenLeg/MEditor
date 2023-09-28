#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Shader.h"
#include "Mesh.h"
#include "UserInterface.h"
#include "ScreenParams.h"
#include "Camera.h"
#include "MakerVAO.h"
#include "Callbacks.h"

int main()
{
	//TODO: window class?
	//Init GLFW and window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "MEditate", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetWindowPos(window, 40, 40);
	glViewport(0, 0, DRAW_SECTION_WIDTH, SCREEN_HEIGHT);

	//Load icon
	GLFWimage image;
	image.pixels = stbi_load("resources/medit.png", &image.width, &image.height, 0, 4);
	glfwSetWindowIcon(window, 1, &image);
	stbi_image_free(image.pixels);

	initImGui(window);

	//Init mesh
	Mesh cube{};
	cube.fillColor = { 1.0f, 0.5f, 0.2f };
	cube.edgesColor = { 1.0f, 1.0f, 1.0f };
	cube.scale = { 1.0f, 1.0f, 1.0f };
	createCubeVAO(cube);
	strcpy_s(cube.name, 20, "Cube");

	POINT_VAO = createPointVAO();

	//Init camera
	Camera camera;
	camera.position = { 0.0f, 0.0f, -3.0f };
	camera.front = { 0.0f, 0.0f, 1.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.right = { 1.0f, 0.0f, 0.0f };
	camera.fov = 80.0f;

	//reachable in callbacks
	glfwSetWindowUserPointer(window, &camera);

	Shader basicShader("basic_vertex.shd", "basic_fragment.shd");

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basicShader.Use();

		basicShader.SetMatrix4("model", getModelMatrix(cube));
		basicShader.SetMatrix4("view", getViewMatrix(camera));
		basicShader.SetMatrix4("proj", getProjectionMatrix(camera));

		drawMesh(cube, basicShader, true, false);
		drawUI(cube);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shutdownImGui();
	glfwTerminate();

	return 0;
}
