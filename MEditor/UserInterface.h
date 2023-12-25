#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>

#include "Node.h"
#include "Mesh.h"
#include "Material.h"
#include "Light.h"
#include "ScreenParams.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

void initImGui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void shutdownImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void beginDraw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void drawNode(Node* node, Node* &selectedNode)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	if (node == selectedNode)
	{
		flags = ImGuiTreeNodeFlags_Selected;
	}

	if (ImGui::TreeNodeEx(node->name, flags))
	{
		if (ImGui::IsItemClicked() || ImGui::IsItemToggledOpen())
		{
			std::cout << "SELECTED " << node->name << std::endl;
			selectedNode = node;
		}

		for (int i = 0; i < node->children.size(); i++)
		{
			drawNode(node->children[i], selectedNode);
		}

		ImGui::TreePop();
	}
}

void drawSceneGraph(Node* root, Node*& selectedNode)
{
	bool imGuiWindowIsClosed;
	ImGui::Begin("SceneGraph", &imGuiWindowIsClosed, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos({ DRAW_SECTION_WIDTH, 0 });
	ImGui::SetWindowSize({ SCREEN_WIDTH - DRAW_SECTION_WIDTH, SCREEN_HEIGHT / 2 });

	ImGui::SetCursorPos({ 10, 20 });

	ImGui::SetWindowFontScale(1.5);

	drawNode(root, selectedNode);
	
	ImGui::End();
}

void drawProperties(Node* selectedNode, Transform* trf, Material* mats, int* materialIndex, int maxMaterials, 
	DirectionalLight* dl, PointLight* pl, SpotLight* sl, bool root=false)
{
	bool imGuiWindowIsClosed;
	ImGui::Begin("Properties", &imGuiWindowIsClosed, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos({ DRAW_SECTION_WIDTH, SCREEN_HEIGHT / 2 });
	ImGui::SetWindowSize({ SCREEN_WIDTH - DRAW_SECTION_WIDTH, SCREEN_HEIGHT / 2 });

	ImGui::SetWindowFontScale(1.5);

	ImGui::BeginTabBar("NodeProperties");

	if (trf)
	{
		if (ImGui::BeginTabItem("Transform"))
		{
			if (!root)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
				ImGui::InputText("Name", selectedNode->name, 20);
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

			//Translate
			ImGui::Text("Location"); ImGui::SameLine();
			float xOffset = ImGui::GetCursorPosX();

			ImGui::DragFloat("X##pos", &trf->position.x, 0.05f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##pos", &trf->position.y, 0.05f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##pos", &trf->position.z, 0.05f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			//Rotate
			ImGui::Text("Rotation"); ImGui::SameLine(); ImGui::SetCursorPosX(xOffset);

			ImGui::DragFloat("X##rot", &trf->orientation.x, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##rot", &trf->orientation.y, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##rot", &trf->orientation.z, 1.0f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			//Scale
			ImGui::Text("Scale"); ImGui::SameLine(); ImGui::SetCursorPosX(xOffset);

			ImGui::DragFloat("X##scl", &trf->scale.x, 0.05f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##scl", &trf->scale.y, 0.05f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##scl", &trf->scale.z, 0.05f);
			ImGui::EndTabItem();
		}
	}

	float changeSpeed = 0.007f;

	if (materialIndex)
	{
		if (ImGui::BeginTabItem("Material"))
		{
			ImGui::SliderInt("Index", materialIndex, 0, maxMaterials);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

			// Ambient
			ImGui::Text("Base Color"); ImGui::SameLine();
			float xOffset = ImGui::GetCursorPosX();

			Material* mat = mats + *materialIndex;

			//Shininess
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
			ImGui::DragFloat("Shine", &mat->shininess, 1.0f);
			ImGui::EndTabItem();
		}
	}

	if (dl)
	{
		if (ImGui::BeginTabItem("Directional Light"))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

			//Direction
			ImGui::Text("Direction"); ImGui::SameLine();
			float xOffset = ImGui::GetCursorPosX();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##ldir", &dl->direction.x, changeSpeed);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##ldir", &dl->direction.y, changeSpeed);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##ldir", &dl->direction.z, changeSpeed);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			// Ambient
			ImGui::Text("Ambient"); ImGui::SameLine();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##lamb", &dl->base.ambient.x, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##lamb", &dl->base.ambient.y, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##lamb", &dl->base.ambient.z, changeSpeed, 0.0f, 1.0f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			//Rotate
			ImGui::Text("Diffuse"); ImGui::SameLine();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##ldif", &dl->base.diffuse.x, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##ldif", &dl->base.diffuse.y, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##ldif", &dl->base.diffuse.z, changeSpeed, 0.0f, 1.0f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			//Scale
			ImGui::Text("Specular"); ImGui::SameLine();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##lspc", &dl->base.specular.x, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##lspc", &dl->base.specular.y, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##lspc", &dl->base.specular.z, changeSpeed, 0.0f, 1.0f);
			ImGui::EndTabItem();
		}
	}

	if (pl)
	{
		if (ImGui::BeginTabItem("Point Light"))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

			//Attenuation
			ImGui::Text("Attenuation"); ImGui::SameLine();
			float xOffset = ImGui::GetCursorPosX();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("C##ldir", &pl->attenuation.constant, changeSpeed);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("L##ldir", &pl->attenuation.linear, changeSpeed);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Q##ldir", &pl->attenuation.quadratic, changeSpeed);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			// Ambient
			ImGui::Text("Ambient"); ImGui::SameLine();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##lamb", &pl->base.ambient.x, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##lamb", &pl->base.ambient.y, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##lamb", &pl->base.ambient.z, changeSpeed, 0.0f, 1.0f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			//Rotate
			ImGui::Text("Diffuse"); ImGui::SameLine();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##ldif", &pl->base.diffuse.x, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##ldif", &pl->base.diffuse.y, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##ldif", &pl->base.diffuse.z, changeSpeed, 0.0f, 1.0f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			//Scale
			ImGui::Text("Specular"); ImGui::SameLine();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##lspc", &pl->base.specular.x, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##lspc", &pl->base.specular.y, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##lspc", &pl->base.specular.z, changeSpeed, 0.0f, 1.0f);
			ImGui::EndTabItem();
		}
	}

	if (sl)
	{
		if (ImGui::BeginTabItem("Specular Light"))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

			//Attenuation
			ImGui::Text("Attenuation"); ImGui::SameLine();
			float xOffset = ImGui::GetCursorPosX();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("C##ldir", &sl->attenuation.constant, changeSpeed);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("L##ldir", &sl->attenuation.linear, changeSpeed);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Q##ldir", &sl->attenuation.quadratic, changeSpeed);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			//Direction
			ImGui::Text("Direction"); ImGui::SameLine();
			
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##ldir", &sl->direction.x, changeSpeed);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##ldir", &sl->direction.y, changeSpeed);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##ldir", &sl->direction.z, changeSpeed);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			//CutOffs
			ImGui::Text("Cut Offs"); ImGui::SameLine();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("I##lin", &sl->cutOff, changeSpeed);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("O##lout", &sl->outerCutOff, changeSpeed);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			// Ambient
			ImGui::Text("Ambient"); ImGui::SameLine();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##lamb", &sl->base.ambient.x, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##lamb", &sl->base.ambient.y, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##lamb", &sl->base.ambient.z, changeSpeed, 0.0f, 1.0f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			//Rotate
			ImGui::Text("Diffuse"); ImGui::SameLine();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##ldif", &sl->base.diffuse.x, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##ldif", &sl->base.diffuse.y, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##ldif", &sl->base.diffuse.z, changeSpeed, 0.0f, 1.0f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			//Scale
			ImGui::Text("Specular"); ImGui::SameLine();

			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("X##lspc", &sl->base.specular.x, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##lspc", &sl->base.specular.y, changeSpeed, 0.0f, 1.0f);
			ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##lspc", &sl->base.specular.z, changeSpeed, 0.0f, 1.0f);
			ImGui::EndTabItem();
		}
	}

	ImGui::EndTabBar();
	ImGui::End();
}
