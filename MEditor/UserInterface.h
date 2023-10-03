#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>

#include "Node.h"
#include "Mesh.h"
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

void drawProperties(Node* selectedNode, Transform& trf, bool root=false)
{
	bool imGuiWindowIsClosed;
	ImGui::Begin("Properties", &imGuiWindowIsClosed, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos({ DRAW_SECTION_WIDTH, SCREEN_HEIGHT / 2 });
	ImGui::SetWindowSize({ SCREEN_WIDTH - DRAW_SECTION_WIDTH, SCREEN_HEIGHT / 2 });

	ImGui::SetWindowFontScale(1.5);

	ImGui::BeginTabBar("NodeProperties");

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

		ImGui::DragFloat("X##pos", &trf.position.x, 0.05f);
		ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##pos", &trf.position.y, 0.05f);
		ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##pos", &trf.position.z, 0.05f);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

		//Rotate
		ImGui::Text("Rotation"); ImGui::SameLine(); ImGui::SetCursorPosX(xOffset);

		ImGui::DragFloat("X##rot", &trf.orientation.x, 1.0f);
		ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##rot", &trf.orientation.y, 1.0f);
		ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##rot", &trf.orientation.z, 1.0f);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

		//Scale
		ImGui::Text("Scale"); ImGui::SameLine(); ImGui::SetCursorPosX(xOffset);

		ImGui::DragFloat("X##scl", &trf.scale.x, 0.05f);
		ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##scl", &trf.scale.y, 0.05f);
		ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##scl", &trf.scale.z, 0.05f);
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Light"))
		ImGui::EndTabItem();

	ImGui::EndTabBar();
	ImGui::End();
}
