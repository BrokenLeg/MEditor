#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"

const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 900;

const unsigned int DRAW_SECTION_WIDTH = 1200;

unsigned int POINT_VAO;
const glm::vec3 WHITE_COLOR = {1.0f, 1.0f, 1.0f};

struct Mesh
{
    glm::vec3 position; //also represents mesh center
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::vec3 color; //move to materials

    char name[20];
    unsigned int VAO;
    unsigned int indicesCount;
};

void drawMesh(const Mesh& mesh, Shader& shader)
{
    glBindVertexArray(mesh.VAO);
    
    shader.SetVector3f("color", mesh.color);
    glDrawElements(GL_TRIANGLES, mesh.indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(POINT_VAO);
    
    shader.SetVector3f("color", WHITE_COLOR);
    glPointSize(5.0f);

    glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
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

struct Camera
{
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    glm::vec3 position;
    float fov;
};

Camera camera;

glm::mat4 getProjectionMatrix(const Camera& camera)
{
    float aspectRation = (float)DRAW_SECTION_WIDTH / (float)SCREEN_HEIGHT;
    return glm::perspective(camera.fov, aspectRation, 0.1f, 100.0f);
}

glm::mat4 getViewMatrix(const Camera& camera)
{
    return glm::lookAt(camera.position, camera.position + camera.front, camera.up);
}

unsigned int createTriangleVAO()
{
    float vertices[] =
    {
        -1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    unsigned int indices[] =
    {
        0, 1, 2
    };

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

unsigned int createPointVAO()
{
    float vertices[] =
    {
        0.0f, 0.0f, 0.0f
    };

    unsigned int indices[] =
    {
        0
    };

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE)
    {
        return;
    }

    //panning
    if (key == GLFW_KEY_A)
    {
        camera.position -= camera.right * 0.1f;
    }

    if (key == GLFW_KEY_D)
    {
        camera.position += camera.right * 0.1f;
    }

    if (key == GLFW_KEY_W)
    {
        camera.position -= camera.up * 0.1f;
    }

    if (key == GLFW_KEY_S)
    {
        camera.position += camera.up * 0.1f;
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset < 0)
    {
        camera.position -= camera.front * 0.1f;
    }
    else
    {
        camera.position += camera.front * 0.1f;
    }
}

int main()
{
    //Init GLFW and window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "MEditate", NULL, NULL);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    glfwSetKeyCallback(window, keyCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetWindowPos(window, 50, 50);
    glViewport(0, 0, DRAW_SECTION_WIDTH, SCREEN_HEIGHT);

    //Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //Init mesh
    Mesh triangle{};
    triangle.color = {1.0f, 0.5f, 0.2f};
    triangle.scale = {1.0f, 1.0f, 1.0f};
    triangle.VAO = createTriangleVAO(); 
    triangle.indicesCount = 3;
    strcpy_s(triangle.name, 20, "Triangle");
    
    POINT_VAO = createPointVAO();

    //Init camera
    camera.position = {0.0f, 0.0f, -3.0f};
    camera.front = {0.0f, 0.0f, 1.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.right = {1.0f, 0.0f, 0.0f};
    camera.fov = 60.0f;

    Shader sh("basic_vertex.shd", "basic_fragment.shd");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        sh.Use();
        
        sh.SetMatrix4("model", getModelMatrix(triangle));
        sh.SetMatrix4("view", getViewMatrix(camera));
        sh.SetMatrix4("proj", getProjectionMatrix(camera));

        drawMesh(triangle, sh);
        drawUI(triangle);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	return 0;
}