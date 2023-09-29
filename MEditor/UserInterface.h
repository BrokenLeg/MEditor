#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>

#include "Mesh.h"
#include "ScreenParams.h"

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

void drawUI(Mesh& selectedMesh)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	bool imGuiWindowIsClosed;
	ImGui::Begin("SceneGraph", &imGuiWindowIsClosed, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos({ DRAW_SECTION_WIDTH, 0 });
	ImGui::SetWindowSize({ SCREEN_WIDTH - DRAW_SECTION_WIDTH, SCREEN_HEIGHT / 2 });

	ImGui::SetCursorPos({ 10, 20 });

	ImGui::SetWindowFontScale(1.5);
	ImGui::Text(selectedMesh.name);

	ImGui::End();

	ImGui::Begin("Properties", &imGuiWindowIsClosed, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos({ DRAW_SECTION_WIDTH, SCREEN_HEIGHT / 2 });
	ImGui::SetWindowSize({ SCREEN_WIDTH - DRAW_SECTION_WIDTH, SCREEN_HEIGHT / 2 });

	ImGui::SetWindowFontScale(1.5);

	ImGui::InputText("Name", selectedMesh.name, 20);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
	ImGui::Text("Transform");
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

	//Translate
	ImGui::Text("Location"); ImGui::SameLine();
	float xOffset = ImGui::GetCursorPosX();

	ImGui::DragFloat("X##pos", &selectedMesh.position.x, 0.05f);
	ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##pos", &selectedMesh.position.y, 0.05f);
	ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##pos", &selectedMesh.position.z, 0.05f);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

	//Rotate
	ImGui::Text("Rotation"); ImGui::SameLine(); ImGui::SetCursorPosX(xOffset);

	ImGui::DragFloat("X##rot", &selectedMesh.rotation.x, 0.05f);
	ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##rot", &selectedMesh.rotation.y, 0.05f);
	ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##rot", &selectedMesh.rotation.z, 0.05f);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

	//Scale
	ImGui::Text("Scale"); ImGui::SameLine(); ImGui::SetCursorPosX(xOffset);

	ImGui::DragFloat("X##scl", &selectedMesh.scale.x, 0.05f);
	ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Y##scl", &selectedMesh.scale.y, 0.05f);
	ImGui::SetCursorPosX(xOffset); ImGui::DragFloat("Z##scl", &selectedMesh.scale.z, 0.05f);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}