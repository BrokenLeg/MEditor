#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#include <string>

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
#include "Light.h"
#include "Material.h"

bool insideUI = false;
aiNode* selectedNode = nullptr;
aiNode* root;

Node* mselectedNode = nullptr;
Node* mroot;

std::vector<Mesh> meshes;
std::vector<Material> materials;

std::vector<Light> lights;
std::vector<DirectionalLight> dirLights;
std::vector<PointLight> pointLights;
std::vector<SpotLight> spotLights;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool cameraMode = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		cameraMode = !cameraMode;

		if (cameraMode == true)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	if (insideUI)
	{
		return;
	}

	if (!cameraMode)
	{
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

		if (xposIn > DRAW_SECTION_WIDTH)
		{
			insideUI = true;
		}
		else
		{
			insideUI = false;
		}


	if (insideUI)
	{
		return;
	}

	if (!cameraMode)
	{
		return;
	}

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{

	if (!cameraMode)
	{
		return;
	}

	if (insideUI)
	{
		return;
	}

	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void loadMesh(aiMesh* mesh, Mesh& load)
{
	unsigned int vboParamsCount = mesh->mNumVertices * (3 + 3 + 2);
	std::vector<float> vertices(vboParamsCount);
	
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D position = mesh->mVertices[i];
		aiVector3D normal = mesh->mNormals[i];
		aiVector3D texCoords = mesh->mTextureCoords[0][i];
		vertices[8 * i + 0]	= position.x;
		vertices[8 * i + 1] = position.y;
		vertices[8 * i + 2] = position.z;
		vertices[8 * i + 3] = normal.x;
		vertices[8 * i + 4] = normal.y;
		vertices[8 * i + 5] = normal.z;
		vertices[8 * i + 6] = texCoords.x;
		vertices[8 * i + 7] = texCoords.y;
	}

	unsigned int totalIndicesCount = mesh->mNumFaces * 3;
	std::vector<unsigned int> fillIndices(totalIndicesCount);

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
		{
			fillIndices[3 * i + j] = mesh->mFaces[i].mIndices[j];
		}
	}

	unsigned int fillVAO = createVAO(&vertices[0], 8 * sizeof(float) * mesh->mNumVertices,
		&fillIndices[0], sizeof(unsigned int) * totalIndicesCount);

	load.fillVAO = fillVAO;
	load.fillIndicesCount = totalIndicesCount;	
	load.materialIndex = mesh->mMaterialIndex - 1;
}

void loadMaterial(aiMaterial* mat, Material& load, const std::string& dir)
{
	aiString path;
	mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);

	std::string sPath = path.C_Str();
	sPath = dir + sPath;

	std::cout << sPath.c_str() << 1 << std::endl;

	unsigned int texId;
	glGenTextures(1, &texId);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(sPath.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
		{
			format = GL_RED;
		}
		else if (nrComponents == 3)
		{
			format = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		std::cout << "loadeed : " << texId << std::endl;
	}

	load.diffuse_texture = texId;
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

	load->children.resize(root->mNumChildren);
	for (int i = 0; i < root->mNumChildren; i++)
	{
		load->children[i] = new Node();
		load->children[i]->transform = {};
		load->children[i]->transform.scale = {1, 1, 1};
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

void drawNode(Node* node, const glm::mat4& parentTransform,  Mesh* globalMeshes, Shader& shader, bool checkSelected=false, bool outline=false)
{
	glm::mat4 trf = parentTransform;

	if (checkSelected && node == mselectedNode)
	{
		outline = true;
	}

	if (node->type == MESH)
	{
		trf = trf * getModelMatrix(node->transform);
		Mesh& mesh = globalMeshes[node->resourceIndex];

		if (outline)
		{
			mesh.draw(shader, trf * glm::scale(glm::mat4(1.0f), { 1.01f, 1.01f, 1.01f }), materials[mesh.materialIndex], false, false, outline);
		}
		else
		{
			mesh.draw(shader, trf, materials[mesh.materialIndex], false, false, outline);
		}
		
	}

	for (int i = 0; i < node->children.size(); i++)
	{
		drawNode(node->children[i], trf, globalMeshes, shader, checkSelected, outline);
	}
}

void setLights(Node* node, const glm::mat4& parentTransform, Shader& shader)
{
	glm::mat4 trf = parentTransform;
	if (node->type == MESH)
	{
		trf = trf * getModelMatrix(node->transform);	
	}
	else if (node->type == LIGHT)
	{
		std::string uniformLight = "l[" + std::to_string(node->resourceIndex) + "].";
		shader.SetVector3f((uniformLight + "pos").c_str(), trf * glm::vec4(node->transform.position, 1));

		Light l = lights[node->resourceIndex];
		shader.SetVector3f((uniformLight + "amb").c_str(), l.ambient);
		shader.SetVector3f((uniformLight + "dif").c_str(), l.diffuse);
		shader.SetVector3f((uniformLight + "spec").c_str(), l.specular);
	}
	else if (node->type == DIR_LIGHT)
	{
		
		std::string uniformLight = "dirLights[" + std::to_string(node->resourceIndex) + "].";
		DirectionalLight light = dirLights[node->resourceIndex];

		shader.SetVector3f((uniformLight + "direction").c_str(), light.direction);
		shader.SetVector3f((uniformLight + "ambient").c_str(), light.base.ambient);
		shader.SetVector3f((uniformLight + "diffuse").c_str(), light.base.diffuse);
		shader.SetVector3f((uniformLight + "specular").c_str(), light.base.specular);
	}
	else if (node->type == POINT_LIGHT)
	{
		std::string uniformLight = "pointLights[" + std::to_string(node->resourceIndex) + "].";
		PointLight light = pointLights[node->resourceIndex];

		shader.SetVector3f((uniformLight + "position").c_str(), trf * glm::vec4(node->transform.position, 1));

		shader.SetVector3f((uniformLight + "ambient").c_str(), light.base.ambient);
		shader.SetVector3f((uniformLight + "diffuse").c_str(), light.base.diffuse);
		shader.SetVector3f((uniformLight + "specular").c_str(), light.base.specular);

		shader.SetFloat((uniformLight + "constant").c_str(), light.attenuation.constant);
		shader.SetFloat((uniformLight + "linear").c_str(), light.attenuation.linear);
		shader.SetFloat((uniformLight + "quadratic").c_str(), light.attenuation.quadratic);
	}
	else if (node->type == SPOT_LIGHT)
	{
		std::string uniformLight = "spotLights[" + std::to_string(node->resourceIndex) + "].";
		SpotLight light = spotLights[node->resourceIndex];

		shader.SetVector3f((uniformLight + "position").c_str(), trf * glm::vec4(node->transform.position, 1));
		shader.SetVector3f((uniformLight + "direction").c_str(), light.direction);

		shader.SetFloat((uniformLight + "cutOff").c_str(), glm::cos(glm::radians(light.cutOff)));
		shader.SetFloat((uniformLight + "outerCutOff").c_str(), glm::cos(glm::radians(light.outerCutOff)));

		shader.SetVector3f((uniformLight + "ambient").c_str(), light.base.ambient);
		shader.SetVector3f((uniformLight + "diffuse").c_str(), light.base.diffuse);
		shader.SetVector3f((uniformLight + "specular").c_str(), light.base.specular);

		shader.SetFloat((uniformLight + "constant").c_str(), light.attenuation.constant);
		shader.SetFloat((uniformLight + "linear").c_str(), light.attenuation.linear);
		shader.SetFloat((uniformLight + "quadratic").c_str(), light.attenuation.quadratic);
	}

	for (int i = 0; i < node->children.size(); i++)
	{
		setLights(node->children[i], trf, shader);
	}
}

GLFWwindow* initGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "MEditate", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.0, 1.0);

	glfwSetCursorPosCallback(window, mouse_callback);
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

	return window;
}

unsigned int loadCubemap(const std::vector<std::string>& faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

int main()
{
	GLFWwindow* window = initGLFW();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Shader basicShader("basic_vertex.shd", "basic_fragment.shd");
	Shader outliningShader("basic_vertex.shd", "outlining_fragment.shd");
	Shader skyboxShader("skybox_vertex.shd", "skybox_fragment.shd");

	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile("resources/tree/christmas_tree.obj", aiProcess_Triangulate);
	meshes.resize(scene->mNumMeshes);
	materials.resize(scene->mNumMaterials);

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		meshes[i] = {};
		loadMesh(scene->mMeshes[i], meshes[i]);
	}

	for (int i = 1; i < scene->mNumMaterials; i++)
	{
		materials[i-1] = {};
		materials[i-1].shininess = 128.0f;
		loadMaterial(scene->mMaterials[i], materials[i-1], "./resources/tree/");
	}

	basicShader.Use();

	root = scene->mRootNode;
	root->mName = "Root";
	
	mroot = new Node;
	mroot->type = MESH;
	mroot->transform = {};
	mroot->transform.scale = { 1, 1, 1 };
	loadScene(root, mroot);

	Node* l = new Node;
	strcpy_s(l->name, 50, "DirLight");
	l->transform = {};
	l->transform.position = { 0.0f, -2.0f, -1.0f };
	l->transform.scale = { 1, 1, 1 };
	l->type = DIR_LIGHT;
	l->resourceIndex = 0;

	DirectionalLight dl{};
	dl.base.ambient = { 0.2f, 0.2f, 0.2f };
	dl.base.diffuse = { 0.5f, 0.5f, 0.5f };
	dl.base.specular = { 1.0f, 1.0f, 1.0f };
	dl.direction = { 0, -1.0f, 0 };
	dirLights.push_back(dl);

	mroot->children.push_back(l);

	Node* pl = new Node;
	strcpy_s(pl->name, 50, "PointLight");
	pl->transform = {};
	pl->transform.position = { 0.0f, -2.0f, -10.0f };
	pl->transform.scale = { 1, 1, 1 };
	pl->type = POINT_LIGHT;
	pl->resourceIndex = 0;

	PointLight pll{};
	pll.base.ambient = { 0.2f, 0.2f, 0.2f };
	pll.base.diffuse = { 0.5f, 0.5f, 0.5f };
	pll.base.specular = { 1.0f, 1.0f, 1.0f };
	
	pll.attenuation.constant = 1.0f;
	pll.attenuation.linear = 0.0f;
	pll.attenuation.quadratic = 0.0f;

	pointLights.push_back(pll);
	mroot->children.push_back(pl);

	Node* sl = new Node;
	strcpy_s(sl->name, 50, "SpotLight");
	sl->transform = {};
	sl->transform.position = { 0.0f, 0.0, 0.0f };
	sl->transform.scale = { 1, 1, 1 };
	sl->type = SPOT_LIGHT;
	sl->resourceIndex = 0;

	SpotLight sll{};
	sll.base.ambient = { 1.0f, 0.0f, 0.0f };
	sll.base.diffuse = { 0.5f, 0.0f, 0.0f };
	sll.base.specular = { 1.0f, 1.0f, 1.0f };

	sll.attenuation.constant = 0.1f;
	sll.attenuation.linear = 0.01f;
	sll.attenuation.quadratic = 0.0f;

	sll.cutOff = 5.0f;
	sll.outerCutOff = 6.0f;

	sll.direction = {-1.0f, -1.0f, -1.0f};

	spotLights.push_back(sll);
	mroot->children.push_back(sl);

	imp.FreeScene();

	//glfwSetWindowUserPointer(window, &basicShader);

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces
	{
		"skybox/px.png",
		"skybox/nx.png",
		"skybox/py.png",
		"skybox/ny.png",
		"skybox/pz.png",
		"skybox/nz.png"
	};
	//stbi_set_flip_vertically_on_load(false);
	unsigned int cubemapTexture = loadCubemap(faces);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glStencilMask(0x00);
		basicShader.Use();

		basicShader.SetMatrix4("view", camera.GetViewMatrix());

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		basicShader.SetMatrix4("proj", projection);

		basicShader.SetVector3f("viewPos", camera.Position);

		setLights(mroot, getModelMatrix(mroot->transform), basicShader);
		drawNode(mroot, getModelMatrix(mroot->transform), &meshes[0], basicShader);

		beginDraw();
		drawSceneGraph(mroot, mselectedNode);

		if (mselectedNode)
		{
			int* index = nullptr;
			DirectionalLight* dirLight = nullptr;
			PointLight* pointLight = nullptr;
			SpotLight* spotLight = nullptr;

			if (mselectedNode->type == MESH)
			{
				Mesh& mesh = meshes[mselectedNode->resourceIndex];
				index = &mesh.materialIndex;
			}
			else if (mselectedNode->type == DIR_LIGHT)
			{
				dirLight = &dirLights[mselectedNode->resourceIndex];
			}
			else if (mselectedNode->type == POINT_LIGHT)
			{
				pointLight = &pointLights[mselectedNode->resourceIndex];
			}
			else if (mselectedNode->type == SPOT_LIGHT)
			{
				spotLight = &spotLights[mselectedNode->resourceIndex];
			}

			drawProperties(mselectedNode, &mselectedNode->transform, &materials[0], index, materials.size() - 1, dirLight, pointLight, spotLight);


		}

		render();

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.Use();
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.SetMatrix4("view", view);
		skyboxShader.SetMatrix4("projection", projection);

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shutdownImGui();
	glfwTerminate();

	return 0;
}
