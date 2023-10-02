#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"
#include "UserInterface.h"
#include "ScreenParams.h"
#include "Camera.h"
#include "MakerVAO.h"
#include "Callbacks.h"

#include "Node.h"

bool insideUI = false;
aiNode* selectedNode = nullptr;
aiNode* root;
std::vector<Mesh> meshes;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (selectedNode == nullptr)
	{
		return;
	}

	if (action == GLFW_RELEASE)
	{
		return;
	}

	float angle = 0.0f;
	glm::vec3 axis{};

	if (key == GLFW_KEY_A)
	{
		angle = 3.0f;
		axis = { 0, 1, 0 };
	}

	if (key == GLFW_KEY_D)
	{
		axis = { 0, 1, 0 };
		angle = -3.0f;
	}

	if (key == GLFW_KEY_W)
	{
		angle = 3.0f;
		axis = { 1, 0, 0 };
	}

	if (key == GLFW_KEY_S)
	{
		axis = { 1, 0, 0 };
		angle = -3.0f;
	}

	if (key == GLFW_KEY_Q)
	{
		angle = -3.0f;
		axis = { 0, 0, 1 };
	}

	if (key == GLFW_KEY_E)
	{
		axis = { 0, 0, 1 };
		angle = 3.0f;
	}

	if (selectedNode->mNumMeshes)
	{
		applyGlobalRotation(meshes[selectedNode->mMeshes[0]].trf, axis, angle);
	}
	else
	{
		applyGlobalRotation(meshes[meshes.size() - 1].trf, axis, angle);
	}
}



void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (xpos > DRAW_SECTION_WIDTH)
	{
		insideUI = true;
	}
	else
	{
		insideUI = false;
	}
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (insideUI)
	{
		return;
	}

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

void loadMesh(aiMesh* mesh, Mesh& load)
{
	//for every Face i've got 3 indices -> total = mNumFaces * 3
	unsigned int vboParamsCount = mesh->mNumVertices * (3 + 3);
	std::vector<float> vertices(vboParamsCount);
	
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D position = mesh->mVertices[i];
		aiVector3D normal = mesh->mNormals[i];
		vertices[6 * i]		= position.x;
		vertices[6 * i + 1] = position.y;
		vertices[6 * i + 2] = position.z;
		vertices[6 * i + 3] = normal.x;
		vertices[6 * i + 4] = normal.y;
		vertices[6 * i + 5] = normal.z;
	}

	unsigned int totalIndicesCount = mesh->mNumFaces * 3;
	std::vector<unsigned int> fillIndices(totalIndicesCount);

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
		{
			fillIndices[3 * i + j] = mesh->mFaces[i].mIndices[j];

			//edgesIndices.push_back(mesh->mFaces[i].mIndices[j]);
			//edgesIndices.push_back(mesh->mFaces[i].mIndices[(j + 1) % 3]);
		}
	}

	unsigned int fillVAO = createVAO(&vertices[0], 6 * sizeof(float) * mesh->mNumVertices,
		&fillIndices[0], sizeof(unsigned int) * totalIndicesCount);

	//unsigned int edgesVAO = createVAO((float*)mesh->mVertices, sizeof(aiVector3D) * mesh->mNumVertices,
	//	&edgesIndices[0], sizeof(unsigned int) * edgesIndices.size());

	load.fillVAO = fillVAO;
	load.fillIndicesCount = totalIndicesCount;
	//loaded.edgesVAO = edgesVAO;
	//loaded.edgesIndicesCount = edgesIndices.size();
	//strcpy_s(load.name, MAX_NAME_LENGTH, mesh->mName.C_Str());
}

void extractTransform(Transform& trf, const glm::mat4& mat4)
{
	glm::vec3 translate = mat4[3];
	
	glm::vec3 first = mat4[0];
	glm::vec3 second = mat4[1];
	glm::vec3 third = mat4[2];
	
	float scaleX = glm::length(first);
	float scaleY = glm::length(second);
	float scaleZ = glm::length(third);

	glm::vec3 scale = {scaleX, scaleY, scaleZ};

	first  = first  / scaleX;
	second = second / scaleY;
	third  = third  / scaleZ;
}

void loadScene(aiNode* root, Node*& load)
{
	strcpy_s(load->name, 50, root->mName.C_Str());
	

	if (root->mNumMeshes)
	{
		load->type = MESH;
		load->resourceIndex = root->mMeshes[0];
	}
	else
	{
		load->type = NONE;
	}

	load->children.reserve(root->mNumChildren);
	for (int i = 0; i < root->mNumChildren; i++)
	{
		loadScene(root->mChildren[i], load->children[i]);
	}
}

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
{
	glm::mat4 to;

	to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
	to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
	to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
	to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

	return to;
}

void drawNode(aiNode* node, const glm::mat4& parentTransform,  Mesh* globalMeshes, Shader& shader)
{
	glm::mat4 trf = parentTransform;

	if (node->mNumMeshes)
	{
		trf = getModelMatrix(globalMeshes[node->mMeshes[0]].trf) * trf;
		drawMesh(globalMeshes[node->mMeshes[0]], shader, trf, false, false);
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		drawNode(node->mChildren[i], trf, globalMeshes, shader);
	}
}

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
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	glfwSetCursorPosCallback(window, cursor_position_callback);
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
	//Init point
	POINT_VAO = createPointVAO();

	//Init camera
	Camera camera;
	camera.position = { 0.0f, 0.0f, -3.0f };
	camera.front = { 0.0f, 0.0f, 1.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.right = { 1.0f, 0.0f, 0.0f };
	camera.fov = 80.0f;
	glfwSetWindowUserPointer(window, &camera);
	//reachable in callbacks
	
	Shader basicShader("basic_vertex.shd", "basic_fragment.shd");

	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile("resources/back.dae", aiProcess_Triangulate);
	meshes.resize(scene->mNumMeshes);

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		meshes[i] = {};
		meshes[i].fillColor = { 0.5f, 0.5f, 0.5f };
		meshes[i].edgesColor = { 1.0f, 1.0f, 1.0f };
		meshes[i].trf.scale = { 1.0f, 1.0f, 1.0f };
		loadMesh(scene->mMeshes[i], meshes[i]);
	}

	Mesh rootMesh{};
	rootMesh.trf.scale = { 1.0f, 1.0f, 1.0f };
	meshes.push_back(rootMesh);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//int selectedMesh = 0;

	root = scene->mRootNode;
	root->mName = "Root";

	basicShader.Use();
	basicShader.SetVector3f("lightPos", {0.0f, -2.0f, -1.0f});

	//imp.FreeScene();
	//need to move scene to my tree-struct to be able to modify
	//need to call freeScene() not to use extra memory

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basicShader.Use();

		//don't have scene graph and local trf yet
		basicShader.SetMatrix4("view", getViewMatrix(camera));
		basicShader.SetMatrix4("proj", getProjectionMatrix(camera));

		drawNode(root, getModelMatrix(meshes[scene->mNumMeshes].trf), &meshes[0], basicShader);

		beginDraw();
		drawSceneGraph(root, selectedNode);

		if (selectedNode)
		{
			if (selectedNode == root)
			{
				drawProperties(selectedNode, meshes[scene->mNumMeshes].trf, true);
			}
			else
			{
				drawProperties(selectedNode, meshes[selectedNode->mMeshes[0]].trf);
			}
		}
		
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shutdownImGui();
	glfwTerminate();

	return 0;
}
